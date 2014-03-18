#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "vfmdcommonregexps.h"
#include "vfmdelementtreenodestack.h"
#include "orderedlisthandler.h"
#include "unorderedlisthandler.h"
#include "core/vfmdblockutils.h"

void ParagraphHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(firstLine);
    UNUSED_ARG(nextLine);
    ParagraphLineSequence *paragraphLineSequence = new ParagraphLineSequence(lineSequence);
    lineSequence->setChildSequence(paragraphLineSequence);
}

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
    , m_containingBlockType(VfmdConstants::UNDEFINED_BLOCK_ELEMENT)
    , m_lines(new VfmdPointerArray<const VfmdByteArray>(128))
    , m_isAtEndOfParagraph(false)
    , m_isLookingAhead(false)
    , m_lookaheadLines(0)
{
    const VfmdBlockLineSequence *containingBlockSequence = (parent? parent->parentLineSequence() : 0);
    if (containingBlockSequence) {
        m_containingBlockType = containingBlockSequence->elementType();
    }
}

ParagraphLineSequence::~ParagraphLineSequence()
{
    if (m_lines) {
        m_lines->freeItemsAndClear();
    }
    delete m_lines;
    delete m_lookaheadLines;
}

static void appendLines(VfmdPointerArray<const VfmdByteArray> *dst, VfmdPointerArray<const VfmdLine> *src)
{
    if (dst && src && (src->size() > 0)) {
        unsigned int sz = src->size();
        for (unsigned int i = 0; i < sz; i++) {
            dst->append(src->itemAt(i)->content().copy());
        }
    }
}

static bool isPotentialEndOfParagraph(const VfmdLine *nextLine, int containingBlockType)
{
    if (nextLine == 0 || nextLine->isBlankLine()) {
        return true;
    }

    if (isHorizontalRuleLine(nextLine->content())) {
        return true;
    }

    const char firstNonSpaceByte = nextLine->firstNonSpace();

    if ((containingBlockType == VfmdConstants::BLOCKQUOTE_ELEMENT) &&
        (firstNonSpaceByte == '>')) {
        return true;
    }

    if ((containingBlockType == VfmdConstants::UNORDERED_LIST_ELEMENT) &&
        (firstNonSpaceByte == '*' || firstNonSpaceByte == '+' || firstNonSpaceByte == '-')) {
        VfmdRegexp reUnorderedListStarter = VfmdCommonRegexps::unorderedListStarter();
        if (reUnorderedListStarter.matches(nextLine->content())) {
            return true;
        }
    }

    if ((containingBlockType == VfmdConstants::ORDERED_LIST_ELEMENT) &&
        (firstNonSpaceByte >= '0' && firstNonSpaceByte <= '9')) {
        VfmdRegexp reOrderedListStarter = VfmdCommonRegexps::orderedListStarter();
        if (reOrderedListStarter.matches(nextLine->content())) {
            return true;
        }
    }

    return false;
}

void ParagraphLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);

    if (!m_isLookingAhead) {

        // Not in lookahead mode
        m_lines->append(currentLine->content().copy());
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine->content(), &m_htmlStateWatcher);
        HtmlStateWatcher::State htmlState = m_htmlStateWatcher.state();
#else
        HtmlStateWatcher::State htmlState = HtmlStateWatcher::TEXT_STATE;
#endif
        bool isPotentialEnd = isPotentialEndOfParagraph(nextLine, m_containingBlockType);
        if (isPotentialEnd) {
            if (htmlState == HtmlStateWatcher::TEXT_STATE) {
                m_isAtEndOfParagraph = true;
                return;
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
            } else if ((htmlState == HtmlStateWatcher::INDETERMINATE_STATE) ||
                       (htmlState == HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE)) {
                // Enter lookahead mode
                m_isLookingAhead = true;
                m_htmlStateWatcher.beginLookahead();
                assert(m_lookaheadLines == 0);
                m_lookaheadLines = new VfmdPointerArray<const VfmdLine>(128);
                return;
            } else {
#endif
                // No other states are possible at this point
                assert(false);
            }
        }

    } else {

#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
        // In lookahead mode
        assert(m_lookaheadLines);
        m_lookaheadLines->append(currentLine->copy());
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine->content(), &m_htmlStateWatcher);
        HtmlStateWatcher::State state = m_htmlStateWatcher.state();
        if ((state == HtmlStateWatcher::HTML_COMMENT_STATE) ||
            (state == HtmlStateWatcher::HTML_TAG_STATE) ||
            (state == HtmlStateWatcher::CONTENT_OF_WELL_FORMED_VERBATIM_HTML_ELEMENT_STATE)) {
            // Paragraph should not end at the point where lookahead started
            appendLines(m_lines, m_lookaheadLines); // Consume lookahead lines
            m_lookaheadLines->freeItemsAndClear();
            m_htmlStateWatcher.endLookahead(true /* consumeLookaheadLines */);
            HtmlStateWatcher::State updatedHtmlState = m_htmlStateWatcher.state();
            if ((updatedHtmlState == HtmlStateWatcher::INDETERMINATE_STATE) ||
                (updatedHtmlState == HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE)) {
                // Begin a new lookahead from (and including) the current line
                m_htmlStateWatcher.beginLookahead();
            } else {
                delete m_lookaheadLines;
                m_lookaheadLines = 0;
                m_isLookingAhead = false;
                if (updatedHtmlState == HtmlStateWatcher::TEXT_STATE) {
                    m_isAtEndOfParagraph = isPotentialEndOfParagraph(nextLine, m_containingBlockType);
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
#endif

    }

}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return m_isAtEndOfParagraph;
}

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    unsigned int sz = m_lines->size();
    unsigned int textSize = 0;
    for (unsigned int i = 0; i < sz; i++) {
        textSize += m_lines->itemAt(i)->size() + 1;
    }
    VfmdByteArray text;
    text.reserve(textSize);
    for (unsigned int i = 0; i < sz; i++) {
        const VfmdByteArray *line = m_lines->itemAt(i);
        text.append(*line);
        text.appendByte(0x0a /* LF */);
        delete line;
    }
    assert(text.size() == text.capacity());
    m_lines->clear();
    text.trim();

    VfmdElementTreeNode *paragraphNode = new ParagraphTreeNode();
    VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(text, registry());
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
