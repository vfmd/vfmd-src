#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "vfmdcommonregexps.h"
#include "vfmdelementtreenodestack.h"
#include "orderedlisthandler.h"
#include "unorderedlisthandler.h"

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
    m_text.clear();
    m_text.squeeze();
    delete m_lookaheadLines;
}

static void appendLines(VfmdByteArray *dst, VfmdPointerArray<const VfmdLine> *src)
{
    if (dst && src && (src->size() > 0)) {
        unsigned int sz = src->size();
        for (unsigned int i = 0; i < sz; i++) {
            dst->append(src->itemAt(i)->content());
            dst->appendByte('\n');
        }
    }
}

static bool isPotentialEndOfParagraph(const VfmdLine *nextLine, int containingBlockType)
{
    if (nextLine == 0 || nextLine->isBlankLine()) {
        return true;
    }

    const char firstNonSpaceByte = nextLine->firstNonSpace();

    if ((containingBlockType == VfmdConstants::BLOCKQUOTE_ELEMENT) &&
        (firstNonSpaceByte == '>')) {
        return true;
    }

    if (containingBlockType == VfmdConstants::UNORDERED_LIST_ELEMENT ||
        containingBlockType == VfmdConstants::ORDERED_LIST_ELEMENT) {
        if ((firstNonSpaceByte == '*' || firstNonSpaceByte == '+' || firstNonSpaceByte == '-') &&
            nextLine->matches(VfmdCommonRegexps::unorderedListStarter())) {
            return true;
        }
        if ((firstNonSpaceByte >= '0' && firstNonSpaceByte <= '9') &&
            nextLine->matches(VfmdCommonRegexps::orderedListStarter())) {
            return true;
        }
    }

    if (nextLine->isHorizontalRuleLine()) {
        return true;
    }

    return false;
}

#ifdef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH

// A simple implementation that doesnot worry about inline HTML

void ParagraphLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    m_text.append(currentLine->content());
    m_text.appendByte('\n');
}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    return (isPotentialEndOfParagraph(nextLine, m_containingBlockType));
}

#else

// A more complex implementation that worries about inline HTML

void ParagraphLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);

    if (!m_isLookingAhead) {

        // Not in lookahead mode
        m_text.append(currentLine->content());
        m_text.appendByte('\n');

        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine->content(), &m_htmlStateWatcher);
        HtmlStateWatcher::State htmlState = m_htmlStateWatcher.state();

        bool isPotentialEnd = isPotentialEndOfParagraph(nextLine, m_containingBlockType);
        if (isPotentialEnd) {
            if (htmlState == HtmlStateWatcher::TEXT_STATE) {
                m_isAtEndOfParagraph = true;
                return;
            } else if ((htmlState == HtmlStateWatcher::INDETERMINATE_STATE) ||
                       (htmlState == HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE)) {
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
        m_lookaheadLines->append(currentLine->copy());
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLine->content(), &m_htmlStateWatcher);
        HtmlStateWatcher::State state = m_htmlStateWatcher.state();
        if ((state == HtmlStateWatcher::HTML_COMMENT_STATE) ||
            (state == HtmlStateWatcher::HTML_TAG_STATE) ||
            (state == HtmlStateWatcher::CONTENT_OF_WELL_FORMED_VERBATIM_HTML_ELEMENT_STATE)) {
            // Paragraph should not end at the point where lookahead started
            appendLines(&m_text, m_lookaheadLines); // Consume lookahead lines
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
    }

}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return m_isAtEndOfParagraph;
}

#endif

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    m_text.squeeze();
    m_text.trim();
    ParagraphTreeNode *paragraphNode = new ParagraphTreeNode();
    VfmdSpanElementsProcessor spanElementsProcessor(m_text, registry());
    VfmdElementTreeNode *spanParseTree = spanElementsProcessor.parseTree();
    bool ok = paragraphNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    if (spanElementsProcessor.isNonPhrasingHtmlTagSeen() ||
        spanElementsProcessor.isMismatchedHtmlTagSeen() ||
        spanElementsProcessor.isHtmlCommentSeen()) {
        paragraphNode->setShouldAvoidWrappingInHtmlPTag(true);
    }
    return paragraphNode;
}

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfBlock() const
{
    return m_lookaheadLines;
}

ParagraphTreeNode::ParagraphTreeNode()
    : m_shouldAvoidWrappingInHtmlPTag(false)
{
}

void ParagraphTreeNode::setShouldAvoidWrappingInHtmlPTag(bool avoidPTag)
{
    m_shouldAvoidWrappingInHtmlPTag = avoidPTag;
}

void ParagraphTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                   VfmdOutputDevice *outputDevice,
                                   VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {

        bool canEncloseContentInPTags = (!m_shouldAvoidWrappingInHtmlPTag);
        bool isContainedInListItem = false;
        bool isContainedInTopPackedListItem = false;
        bool isContainedInBottomPackedListItem = false;
        const VfmdElementTreeNode *parentNode = ancestorNodes->topNode();
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

        if (canEncloseContentInPTags) {
            // If this paragraph is part of a list item, there are additional things to be
            // considered in deciding whether the content can be enclosed in p tags
            if (isContainedInTopPackedListItem) {
                bool firstBlockInParent = (parentNode && parentNode->firstChildNode() == this);
                if (firstBlockInParent) {
                    canEncloseContentInPTags = false;
                }
            }
            if (isContainedInBottomPackedListItem) {
                bool secondBlockInParent = (parentNode && parentNode->firstChildNode() &&
                                            parentNode->firstChildNode()->nextNode() == this);
                bool lastBlockInParent = (parentNode && parentNode->lastChildNode() == this);
                if (lastBlockInParent && !secondBlockInParent) {
                    canEncloseContentInPTags = false;
                }
            }
        }

        if (canEncloseContentInPTags) {
            if (!isContainedInListItem) {
                if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
                    renderHtmlIndent(outputDevice, ancestorNodes);
                }
            }
            outputDevice->write("<p>");
        }
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if (canEncloseContentInPTags) {
            outputDevice->write("</p>\n");
        }

    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
