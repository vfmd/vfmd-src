#include <stdio.h>
#include "vfmdspanelementsprocessor.h"
#include "vfmdlinearray.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdelementregistry.h"
#include "vfmdspantagstack.h"
#include "vfmdscopedpointer.h"
#include "spanelements/vfmdspanelementhandler.h"
#include "vfmdelementtreenode.h"

static bool isValidUtf8StartByte(unsigned char c)
{
    if (c < 128) { // A single-byte UTF-8 character
        return true;
    }
    if (c < 0xc0) { // A UTF-8 continuation byte
       return false;
    }
    if (c >= 0xfe) { // Bytes 0xfe and 0xff are invalid in UTF-8
        return false;
    }
    return true;
}

static void closeTextFragmentIfOpen(VfmdLineArrayIterator *textFragmentStart,
                                    const VfmdLineArrayIterator &iterator,
                                    VfmdSpanTagStack *stack)
{
    if (textFragmentStart->isValid()) {
        if (textFragmentStart->isBefore(&iterator)) {
            VfmdByteArray textFragment = textFragmentStart->bytesTill(iterator);
            stack->topNode()->appendToContainedElements(textFragment);
        }
        (*textFragmentStart) = VfmdLineArrayIterator(); // invalid iterator
    }
}

static bool applySpanHandlerOnLineArrayIterator(VfmdSpanElementHandler *spanHandler, int triggerOptions,
                                                VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack,
                                                VfmdLineArrayIterator *textFragmentStart)
{
    bool shouldInvokeEvenIfEscaped = ((triggerOptions & VfmdElementRegistry::TRIGGER_EVEN_IF_ESCAPED) ==
                                      VfmdElementRegistry::TRIGGER_EVEN_IF_ESCAPED);
    if (!shouldInvokeEvenIfEscaped && iterator->isNextByteEscaped()) {
        return false;
    }
    bool shouldInvokeBeforeTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE) ==
                                          VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE);
    bool shouldInvokeAtTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE) ==
                                      VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);

    if (shouldInvokeBeforeTriggerByte) {

        VfmdLineArrayIterator fromIter;
        if (textFragmentStart->isValid()) {
            fromIter = (*textFragmentStart);
        } else {
            fromIter = (*iterator);
        }
        VfmdLineArrayIterator toIter = (*iterator);
        if (!toIter.isAtEnd()) {
            toIter.moveForward(1); // move past the trigger byte
        }
        bool identified = spanHandler->identifySpanTagStartingBetween(&fromIter, &toIter, stack);

        assert((*iterator)          <= toIter);
        assert(fromIter             <= toIter);

        if (  ((*iterator)          <= toIter)   ||
              (fromIter             <= toIter)) {
            return false;
        }

        if (identified) {

            // Ensure toIter is not in the middle of a UTF-8 sequence
            // (And in debug mode, abort if that happens)
            char nextByte = (toIter.isAtEnd()? 0 : toIter.nextByte());
            assert(isValidUtf8StartByte(nextByte));
            while ((!toIter.isAtEnd()) && (!isValidUtf8StartByte(nextByte))) {
                toIter.moveForward(1);
                nextByte = (toIter.isAtEnd()? 0 : toIter.nextByte());
            }

            assert(fromIter < toIter);
            if (fromIter >= toIter) {
                return false;
            }
            iterator->moveTo(toIter);
            return true;
        }

    } else if (shouldInvokeAtTriggerByte) {

        VfmdLineArrayIterator endOfTag = (*iterator);
        spanHandler->identifySpanTagStartingAt(&endOfTag, stack);
        bool identified = endOfTag.isAfter(iterator);
        if (identified) {
            iterator->moveTo(endOfTag);
            return true;
        }

    }

    return false;
}

VfmdElementTreeNode* VfmdSpanElementsProcessor::processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry)
{
    VfmdSpanTagStack stack;
    VfmdLineArrayIterator iterator = lineArray->begin();
    VfmdLineArrayIterator textFragmentStart;
    assert(textFragmentStart.isValid() == false); // initially invalid

    while (!iterator.isAtEnd()) {
        char currentByte = iterator.nextByte();
        bool isTagIdentifiedAtCurrentPos = false;

        if (isValidUtf8StartByte(currentByte)) {
            // Ask the span element handlers pertaining to this trigger byte
            int n = registry->spanElementCountForTriggerByte(currentByte);
            if (n > 0) {
                closeTextFragmentIfOpen(&textFragmentStart, iterator, &stack);
            }
            for (int i = 0; i < n; i++) {
                VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
                int triggerOptions = registry->triggerOptionsForTriggerByte(currentByte, i);
                isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, triggerOptions,
                                                                                  &iterator, &stack, &textFragmentStart);
                if (isTagIdentifiedAtCurrentPos) {
                    break;
                }
            }
        } else {
            assert(textFragmentStart.isValid());
        }

        // All span element handlers rejected this position => text fragment
        if (!isTagIdentifiedAtCurrentPos) {
            if (!textFragmentStart.isValid()) { // If no text fragment is open
                // Open a text fragment
                textFragmentStart = iterator;
            }
            assert(textFragmentStart.isValid());
            iterator.moveForward(1);
        }
    }

    closeTextFragmentIfOpen(&textFragmentStart, iterator, &stack);

    VfmdElementTreeNode *parseTree = stack.collapse();
    return parseTree;
}
