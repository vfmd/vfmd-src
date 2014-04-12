#include <stdio.h>
#include "paragraphhandler.h"
#include "core/vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "core/vfmdcommonregexps.h"
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
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    , m_isAtEndOfParagraph(false)
    , m_isLookingAhead(false)
    , m_lookaheadLines(0)
#endif
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
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    delete m_lookaheadLines;
#endif
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

static bool isPotentialEndOfParagraph(const VfmdLine *nextLine, int containingBlockType, bool isInVerbatimHtmlMode)
{
    if (nextLine == 0 || nextLine->isBlankLine()) {
        return true;
    }

    if (isInVerbatimHtmlMode) {
        // If we're in verbatim-html mode, we can end the para only at a blank line.
        // We know that the next line is a non-blank line, so the para can't end here.
        return false;
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
    return (isPotentialEndOfParagraph(nextLine, m_containingBlockType, false));
}

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfBlock()
{
    return 0;
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
        HtmlStateWatcher::TagState tagState = m_htmlStateWatcher.tagState();
        HtmlStateWatcher::VerbatimContainerElementState verbatimContainerElemState = m_htmlStateWatcher.verbatimContainerElementState();
        HtmlStateWatcher::VerbatimStarterElementState verbatimStarterElemState = m_htmlStateWatcher.verbatimStarterElementState();

        bool isInVerbatimHtmlMode = ((verbatimContainerElemState != HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN) ||
                                     (verbatimStarterElemState != HtmlStateWatcher::NO_VERBATIM_STARTER_ELEMENT_SEEN));
        bool isPotentialEnd = isPotentialEndOfParagraph(nextLine, m_containingBlockType, isInVerbatimHtmlMode);
        if (isPotentialEnd) {
            if ((tagState == HtmlStateWatcher::TEXT_STATE) &&
                (verbatimContainerElemState == HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN ||
                 verbatimContainerElemState == HtmlStateWatcher::NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT)) {
                m_isAtEndOfParagraph = true;
                return;
            } else if ((tagState == HtmlStateWatcher::INDETERMINATE_STATE) ||
                       (verbatimContainerElemState == HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE)) {
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
        HtmlStateWatcher::TagState tagState = m_htmlStateWatcher.tagState();
        HtmlStateWatcher::VerbatimContainerElementState verbatimContainerElemState = m_htmlStateWatcher.verbatimContainerElementState();

        if ((tagState == HtmlStateWatcher::HTML_COMMENT_STATE) ||
            (tagState == HtmlStateWatcher::HTML_TAG_STATE) ||
            (verbatimContainerElemState == HtmlStateWatcher::WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT)) {
            // Paragraph should not end at the point where lookahead started
            appendLines(&m_text, m_lookaheadLines); // Consume lookahead lines
            m_lookaheadLines->freeItemsAndClear();
            m_htmlStateWatcher.endLookahead(true /* consumeLookaheadLines */);
            HtmlStateWatcher::TagState updatedTagState = m_htmlStateWatcher.tagState();
            HtmlStateWatcher::VerbatimContainerElementState updatedVerbatimContainerElemState = m_htmlStateWatcher.verbatimContainerElementState();
            HtmlStateWatcher::VerbatimStarterElementState updatedVerbatimStarterElemState = m_htmlStateWatcher.verbatimStarterElementState();
            bool isInVerbatimHtmlMode = ((updatedVerbatimContainerElemState != HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN) ||
                                         (updatedVerbatimStarterElemState != HtmlStateWatcher::NO_VERBATIM_STARTER_ELEMENT_SEEN));

            if ((updatedTagState == HtmlStateWatcher::INDETERMINATE_STATE) ||
                (updatedVerbatimContainerElemState == HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE)) {
                // Begin a new lookahead from (and including) the current line
                m_htmlStateWatcher.beginLookahead();
            } else {
                delete m_lookaheadLines;
                m_lookaheadLines = 0;
                m_isLookingAhead = false;
                if ((updatedTagState == HtmlStateWatcher::TEXT_STATE) &&
                    (updatedVerbatimContainerElemState == HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN ||
                     updatedVerbatimContainerElemState == HtmlStateWatcher::NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT)) {
                    m_isAtEndOfParagraph = isPotentialEndOfParagraph(nextLine, m_containingBlockType, isInVerbatimHtmlMode);

                } else {
                    // No other states are possible at this point
                    assert(false);
                }
            }
            return;
        } else if ((tagState == HtmlStateWatcher::TEXT_STATE) &&
                   (verbatimContainerElemState == HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN ||
                    verbatimContainerElemState == HtmlStateWatcher::NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT)) {
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

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfBlock()
{
    VfmdPointerArray<const VfmdLine> *lines = m_lookaheadLines;
    m_lookaheadLines = 0;
    return lines;
}

#endif

void ParagraphLineSequence::endBlock()
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
    setBlockParseTree(paragraphNode);
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
        bool isSoleContentOfAListItem = false;
        bool isContainedInTopPackedListItem = false;
        bool isContainedInBottomPackedListItem = false;
        const VfmdElementTreeNode *parentNode = ancestorNodes->topNode();
        if (parentNode) {
            if (parentNode->elementType() == VfmdConstants::UNORDERED_LIST_ELEMENT) {
                const UnorderedListItemTreeNode *listItemNode = dynamic_cast<const UnorderedListItemTreeNode *>(parentNode);
                if (listItemNode) {
                    isSoleContentOfAListItem = ((listItemNode->firstChildNode() == this) && (nextNode() == 0));
                    isContainedInTopPackedListItem = listItemNode->isTopPacked();
                    isContainedInBottomPackedListItem = listItemNode->isBottomPacked();
                }
            } else if (parentNode->elementType() == VfmdConstants::ORDERED_LIST_ELEMENT) {
                const OrderedListItemTreeNode *listItemNode = dynamic_cast<const OrderedListItemTreeNode *>(parentNode);
                if (listItemNode) {
                    isSoleContentOfAListItem = ((listItemNode->firstChildNode() == this) && (nextNode() == 0));
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

        if (isSoleContentOfAListItem) {
            // Use a compact output
            if (canEncloseContentInPTags) {
                outputDevice->write("<p>", 3);
            }
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            if (canEncloseContentInPTags) {
                outputDevice->write("</p>", 4);
            }
        } else {
            if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
                renderHtmlIndent(outputDevice, ancestorNodes);
            }
            if (canEncloseContentInPTags) {
                outputDevice->write("<p>", 3);
            }
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            if (canEncloseContentInPTags) {
                outputDevice->write("</p>\n", 5);
            } else {
                outputDevice->write('\n');
            }
        }

    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
