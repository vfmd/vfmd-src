#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "vfmdcommonregexps.h"
#include "vfmdelementtreenodestack.h"
#include "orderedlisthandler.h"
#include "unorderedlisthandler.h"

void ParagraphHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(firstLine);
    UNUSED_ARG(nextLine);
    ParagraphLineSequence *paragraphLineSequence = new ParagraphLineSequence(lineSequence);
    lineSequence->setChildSequence(paragraphLineSequence);
}

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
    , m_isAtEndOfParagraph(false)
    , m_isLookingAhead(false)
    , m_lookaheadLines(0)
{
}

ParagraphLineSequence::~ParagraphLineSequence()
{
}

static void appendToLineArray(VfmdLineArray *lineArray, VfmdPointerArray<const VfmdLine> *lines)
{
    if (lineArray && lines && (lines->size() > 0)) {
        unsigned int sz = lines->size();
        for (unsigned int i = 0; i < sz; i++) {
            lineArray->addLine(*(lines->itemAt(i)));
        }
    }
}

static bool isPotentialEndOfParagraph(const VfmdInputLineSequence *parentSequence, const VfmdLine &nextLine)
{
    if (nextLine.isBlankLine()) {
        return true;
    }
    if (!nextLine.isValid()) {
        return true;
    }
    VfmdRegexp reHorizontalRule = VfmdCommonRegexps::horizontalRule();
    if (reHorizontalRule.matches(nextLine)) {
        return true;
    }

    int containerType = VfmdConstants::UNDEFINED_BLOCK_ELEMENT;
    const VfmdBlockLineSequence *containingBlockSequence = (parentSequence? parentSequence->parentLineSequence() : 0);
    if (containingBlockSequence) {
        containerType = containingBlockSequence->elementType();
    }

    if ((nextLine.firstNonSpace() == '>') &&
        (containerType == VfmdConstants::BLOCKQUOTE_ELEMENT)) {
        return true;
    }

    VfmdRegexp reOrderedListStarter = VfmdCommonRegexps::orderedListStarter();
    VfmdRegexp reUnorderedListStarter = VfmdCommonRegexps::unorderedListStarter();

    if ((reUnorderedListStarter.matches(nextLine)) &&
        (containerType == VfmdConstants::UNORDERED_LIST_ELEMENT)) {
        return true;
    }
    if ((reOrderedListStarter.matches(nextLine)) &&
        (containerType == VfmdConstants::ORDERED_LIST_ELEMENT)) {
        return true;
    }

    return false;
}

void ParagraphLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);

    if (!m_isLookingAhead) {

        // Not in lookahead mode
        m_lineArray.addLine(currentLine);
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine, &m_htmlStateWatcher);
        HtmlStateWatcher::State htmlState = m_htmlStateWatcher.state();
        bool isPotentialEnd = isPotentialEndOfParagraph(parentLineSequence(), nextLine);
        if (isPotentialEnd) {
            if (htmlState == HtmlStateWatcher::TEXT_STATE) {
                m_isAtEndOfParagraph = true;
                return;
            } else if (htmlState == HtmlStateWatcher::INDETERMINATE_STATE) {
                // Enter lookahead mode
                m_isLookingAhead = true;
                m_htmlStateWatcher.beginLookahead();
                assert(m_lookaheadLines == 0);
                m_lookaheadLines = new VfmdPointerArray<const VfmdLine>(128);
                return;
            } else {
                // No other states are possible at this point
                assert(false);
            }
        }

    } else {

        // In lookahead mode
        assert(m_lookaheadLines);
        m_lookaheadLines->append(new VfmdLine(currentLine));
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine, &m_htmlStateWatcher);
        HtmlStateWatcher::State state = m_htmlStateWatcher.state();
        if ((state == HtmlStateWatcher::HTML_COMMENT_STATE) ||
            (state == HtmlStateWatcher::HTML_TAG_STATE)) {
            // Paragraph should not end at the point where lookahead started
            appendToLineArray(&m_lineArray, m_lookaheadLines); // Consume lookahead lines
            m_lookaheadLines->freeItemsAndClear();
            m_htmlStateWatcher.endLookahead(true /* consumeLookaheadLines */);
            HtmlStateWatcher::State updatedHtmlState = m_htmlStateWatcher.state();
            if (updatedHtmlState == HtmlStateWatcher::INDETERMINATE_STATE) {
                // Begin a new lookahead from (and including) the current line
                m_htmlStateWatcher.beginLookahead();
            } else {
                delete m_lookaheadLines;
                m_lookaheadLines = 0;
                m_isLookingAhead = false;
                if (updatedHtmlState == HtmlStateWatcher::TEXT_STATE) {
                    m_isAtEndOfParagraph = isPotentialEndOfParagraph(parentLineSequence(), nextLine);
                } else {
                    // No other states are possible at this point
                    assert(false);
                }
            }
            return;
        } else if (state == HtmlStateWatcher::TEXT_STATE) {
            // Paragraph should have ended at the point where lookahead started
            m_isLookingAhead = false;
            m_htmlStateWatcher.endLookahead(false /* consumeLookaheadLines */);
            m_isAtEndOfParagraph = true;
            return;
        }

    }

}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return m_isAtEndOfParagraph;
}

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    m_lineArray.trim();
    VfmdElementTreeNode *paragraphNode = new ParagraphTreeNode();
    VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
    bool ok = paragraphNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    return paragraphNode;
}

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfBlock() const
{
    return m_lookaheadLines;
}

ParagraphTreeNode::ParagraphTreeNode()
{
}

ParagraphTreeNode::~ParagraphTreeNode()
{
}

void ParagraphTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                   VfmdOutputDevice *outputDevice,
                                   VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {

        bool encloseContentInPTags = true;
        const VfmdElementTreeNode *parentNode = ancestorNodes->topNode();
        bool isContainedInListItem = false;
        bool isContainedInTopPackedListItem = false;
        bool isContainedInBottomPackedListItem = false;
        if (parentNode) {
            if (parentNode->elementType() == VfmdConstants::UNORDERED_LIST_ELEMENT) {
                const UnorderedListItemTreeNode *listItemNode = dynamic_cast<const UnorderedListItemTreeNode *>(parentNode);
                if (listItemNode) {
                    isContainedInListItem = true;
                    isContainedInTopPackedListItem = listItemNode->isTopPacked();
                    isContainedInBottomPackedListItem = listItemNode->isBottomPacked();
                }
            } else if (parentNode->elementType() == VfmdConstants::ORDERED_LIST_ELEMENT) {
                const OrderedListItemTreeNode *listItemNode = dynamic_cast<const OrderedListItemTreeNode *>(parentNode);
                if (listItemNode) {
                    isContainedInListItem = true;
                    isContainedInTopPackedListItem = listItemNode->isTopPacked();
                    isContainedInBottomPackedListItem = listItemNode->isBottomPacked();
                }
            }
        }
        if (isContainedInTopPackedListItem) {
            bool firstBlockInParent = (parentNode && parentNode->firstChildNode() == this);
            if (firstBlockInParent) {
                encloseContentInPTags = false;
            }
        }
        if (isContainedInBottomPackedListItem) {
            bool secondBlockInParent = (parentNode && parentNode->firstChildNode() &&
                                        parentNode->firstChildNode()->nextNode() == this);
            bool lastBlockInParent = (parentNode && parentNode->lastChildNode() == this);
            if (lastBlockInParent && !secondBlockInParent) {
                encloseContentInPTags = false;
            }
        }

        if (encloseContentInPTags) {
            if (!isContainedInListItem) {
                if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
                    renderHtmlIndent(outputDevice, ancestorNodes);
                }
            }
            outputDevice->write("<p>");
        }
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if (encloseContentInPTags) {
            outputDevice->write("</p>\n");
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
