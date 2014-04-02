#include <stdio.h>
#include "vfmdspanelementsprocessor.h"
#include "vfmdelementregistry.h"
#include "vfmdspantagstack.h"
#include "vfmdscopedpointer.h"
#include "vfmdspanelementhandler.h"
#include "vfmdelementtreenode.h"

static inline void closeTextFragmentIfOpen(const VfmdByteArray &text, int currentPos,
                                           int *textFragmentStartPosPtr,
                                           VfmdSpanTagStack *stack)
{
    int textFragmentStartPos = (*textFragmentStartPosPtr);
    if (textFragmentStartPos >= 0) {
        if (textFragmentStartPos < currentPos) {
            VfmdByteArray textFragment = text.mid(textFragmentStartPos, currentPos - textFragmentStartPos);
            stack->topNode()->appendToContainedElements(textFragment);
        }
        (*textFragmentStartPosPtr) = -1;
    }
}

static inline int applySpanHandler(const VfmdByteArray &text, int currentPos,
                                    int *textFragmentStartPosPtr,
                                    VfmdSpanTagStack *stack,
                                    VfmdSpanElementHandler *spanHandler, int triggerOptions)
{
    bool shouldInvokeEvenIfEscaped = ((triggerOptions & VfmdElementRegistry::TRIGGER_EVEN_IF_ESCAPED) ==
                                      VfmdElementRegistry::TRIGGER_EVEN_IF_ESCAPED);
    if (!shouldInvokeEvenIfEscaped && text.isEscapedAtPosition(currentPos)) {
        return false;
    }
    bool shouldInvokeBeforeTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE) ==
                                          VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE);
    bool shouldInvokeAtTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE) ==
                                      VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);

    if (shouldInvokeBeforeTriggerByte) {
        int fromPos = (*textFragmentStartPosPtr);
        if (fromPos < 0) {
            fromPos = currentPos;
        }
        int consumedBytes = spanHandler->identifySpanTagStartingBetween(text, fromPos, currentPos, stack);
        if (consumedBytes > 0) {
            (*textFragmentStartPosPtr) = -1; // The span handler would have handled the text fragment
            return (fromPos + consumedBytes - currentPos);
        }
    } else if (shouldInvokeAtTriggerByte) {
        closeTextFragmentIfOpen(text, currentPos, textFragmentStartPosPtr, stack);
        return spanHandler->identifySpanTagStartingAt(text, currentPos, stack);
    }

    return 0;
}

static void processSpanElements(const VfmdByteArray &text, const VfmdElementRegistry *registry, VfmdSpanTagStack *stack)
{
    int currentPos = 0;
    int endPos = (int) text.size();
    int textFragmentStartPos = -1;

    while (currentPos < endPos) {
        char currentByte = text.byteAt(currentPos);
        bool isTagIdentifiedAtCurrentPos = false;
        assert(text.isUTF8CharStartingAt(currentPos));

        // Ask the span element handlers pertaining to this trigger byte
        int n = registry->spanElementCountForTriggerByte(currentByte);
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
                int triggerOptions = registry->triggerOptionsForTriggerByte(currentByte, i);
                int consumedBytes = applySpanHandler(text, currentPos, &textFragmentStartPos, stack, spanHandler, triggerOptions);
                if (consumedBytes > 0) {
                    currentPos += consumedBytes;
                    while ((currentPos < endPos) && (!text.isUTF8CharStartingAt(currentPos))) {
                        currentPos++;
                    }
                    isTagIdentifiedAtCurrentPos = true;
                    break;
                }
            }
        }

        // All span element handlers rejected this position => text fragment
        if (!isTagIdentifiedAtCurrentPos) {
            if (textFragmentStartPos < 0) { // If no text fragment is open
                // Open a text fragment
                textFragmentStartPos = currentPos;
            }
            assert(textFragmentStartPos >= 0);
            // Fast-forward to the next trigger byte
            currentPos = registry->indexOfTriggerByteIn(text, currentPos + 1);
        }
    }

    closeTextFragmentIfOpen(text, currentPos, &textFragmentStartPos, stack);
    stack->collapse();
}

VfmdSpanElementsProcessor::VfmdSpanElementsProcessor(const VfmdByteArray &text, const VfmdElementRegistry *registry)
{
    processSpanElements(text, registry, &m_stack);
}

VfmdElementTreeNode* VfmdSpanElementsProcessor::parseTree() const
{
    return m_stack.baseNodeContents();
}

bool VfmdSpanElementsProcessor::isNonPhrasingHtmlTagSeen() const
{
    return m_stack.isNonPhrasingHtmlTagSeen();
}

bool VfmdSpanElementsProcessor::isMismatchedHtmlTagSeen() const
{
    return m_stack.isMismatchedHtmlTagSeen();
}

bool VfmdSpanElementsProcessor::isHtmlCommentSeen() const
{
    return m_stack.isHtmlCommentSeen();
}
