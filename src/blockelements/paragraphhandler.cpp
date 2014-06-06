#include <stdio.h>
#include "paragraphhandler.h"
#include "core/vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "core/vfmdcommonregexps.h"
#include "vfmdelementtreenodestack.h"
#include "orderedlisthandler.h"
#include "unorderedlisthandler.h"
#include "vfmdelementregistry.h"

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
    , m_containingBlockType(parent->containingBlockType())
    , m_nextBlockHandler(0)
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    , m_isUsingHtmlStateWatcher(false)
    , m_isAtEndOfParagraph(false)
    , m_isLookingAhead(false)
    , m_lookaheadLines(0)
#endif
{
}

ParagraphLineSequence::~ParagraphLineSequence()
{
    m_text.clear();
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    delete m_lookaheadLines;
#endif
}

bool ParagraphLineSequence::canEndBeforeLine(const VfmdLine *line, bool isInVerbatimHtmlMode)
{
    m_nextBlockHandler = 0;

    if (line == 0 || line->isBlankLine()) {
        return true;
    }

    if (isInVerbatimHtmlMode) {
        // If we're in verbatim-html mode, we can end the para only at a blank line.
        // We know that the next line is a non-blank line, so the para can't end here.
        return false;
    }

    if (line->leadingSpacesCount() >= 4) {
        // Code spans always require a separating blank line
        return false;
    }

    // Check with the block handlers whether any of them want to start a block at 'line'
    const char triggerByte = line->firstNonSpace();
    for (int i = 0; i < registry()->numberOfBlockElementsForTriggerByte(triggerByte); i++) {
        int blockOptions = registry()->blockOptionsForTriggerByteAtIndex(triggerByte, i);
        if ((blockOptions & VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH) == VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH) {
            VfmdBlockElementHandler *blockHandler = registry()->blockElementForTriggerByteAtIndex(triggerByte, i);
            if (blockHandler->isStartOfBlock(line, m_containingBlockType, true)) {
                m_nextBlockHandler = blockHandler;
                return true;
            }
        }
    }

    return false;
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

#ifdef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH

// A simple implementation that doesnot worry about inline HTML

void ParagraphLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    m_text.append(currentLine->content());
    m_text.appendByte('\n');
}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    return (canEndBeforeLine(nextLine, false));
}

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfParagraph()
{
    return 0;
}

#else

// A more complex implementation that worries about inline HTML

void ParagraphLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);

    VfmdByteArray currentLineContent = currentLine->content();

    if (!m_isLookingAhead) {

        // Not in lookahead mode

        if (m_text.isInvalid()) {
            m_text = currentLineContent;
        } else {
            m_text.append(currentLineContent);
        }
        m_text.appendByte('\n');

        if (!m_isUsingHtmlStateWatcher && (currentLineContent.indexOf('<') >= 0)) {
            m_isUsingHtmlStateWatcher = true;
        }
        if (m_isUsingHtmlStateWatcher) {
            m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLineContent, &m_htmlStateWatcher);
        }

        HtmlStateWatcher::TagState tagState = m_htmlStateWatcher.tagState();
        HtmlStateWatcher::VerbatimContainerElementState verbatimContainerElemState = m_htmlStateWatcher.verbatimContainerElementState();
        HtmlStateWatcher::VerbatimStarterElementState verbatimStarterElemState = m_htmlStateWatcher.verbatimStarterElementState();

        bool isInVerbatimHtmlMode = ((verbatimContainerElemState != HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN) ||
                                     (verbatimStarterElemState != HtmlStateWatcher::NO_VERBATIM_STARTER_ELEMENT_SEEN));
        bool isPotentialEnd = canEndBeforeLine(nextLine, isInVerbatimHtmlMode);
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
        m_codeSpanFilter.addFilteredLineToHtmlStateWatcher(currentLineContent, &m_htmlStateWatcher);
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
                    m_isAtEndOfParagraph = canEndBeforeLine(nextLine, isInVerbatimHtmlMode);

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

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return m_isAtEndOfParagraph;
}

VfmdPointerArray<const VfmdLine> *ParagraphLineSequence::linesSinceEndOfParagraph()
{
    VfmdPointerArray<const VfmdLine> *lines = m_lookaheadLines;
    m_lookaheadLines = 0;
    return lines;
}

#endif

void ParagraphLineSequence::endBlock()
{
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

void ParagraphLineSequence::setNextBlockHandler(VfmdBlockElementHandler *handler)
{
    m_nextBlockHandler = handler;
}

VfmdBlockElementHandler *ParagraphLineSequence::nextBlockHandler() const
{
    return m_nextBlockHandler;
}

ParagraphTreeNode::ParagraphTreeNode()
    : m_shouldAvoidWrappingInHtmlPTag(false)
{
}

void ParagraphTreeNode::setShouldAvoidWrappingInHtmlPTag(bool avoidPTag)
{
    m_shouldAvoidWrappingInHtmlPTag = avoidPTag;
}
