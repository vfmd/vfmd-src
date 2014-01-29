#include <stdio.h>
#include "vfmdspanelementsprocessor.h"
#include "vfmdlinearray.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdelementregistry.h"
#include "vfmdspantagstack.h"
#include "vfmdscopedpointer.h"
#include "spanelements/vfmdspanelementhandler.h"

static void closeTextFragmentIfOpen(VfmdLineArrayIterator *textFragmentStart, const VfmdLineArrayIterator &iterator)
{
    if (textFragmentStart->isValid()) {
        if (textFragmentStart->isBefore(&iterator)) {
            VfmdByteArray textFragment = textFragmentStart->bytesTill(iterator);
            printf("TEXTFRAG("); textFragment.print(); printf(")\n");
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

        assert((*textFragmentStart) <= fromIter);
        assert((*iterator)          <= toIter);
        assert(fromIter             <= toIter);

        if (  ((*textFragmentStart) <= fromIter) ||
              ((*iterator)          <= toIter)   ||
              (fromIter             <= toIter)) {
            return false;
        }

        if (identified) {
            assert(fromIter < toIter);
            if (fromIter >= toIter) {
                return false;
            }
            closeTextFragmentIfOpen(textFragmentStart, fromIter);
            VfmdByteArray spanTagText = fromIter.bytesTill(toIter);
            printf("SPANTAG("); spanTagText.print(); printf(")\n");
            iterator->moveTo(toIter);
            return true;
        }

    } else if (shouldInvokeAtTriggerByte) {

        VfmdLineArrayIterator endOfTag = (*iterator);
        spanHandler->identifySpanTagStartingAt(&endOfTag, stack);
        bool identified = endOfTag.isAfter(iterator);
        if (identified) {
            closeTextFragmentIfOpen(textFragmentStart, (*iterator));
            VfmdByteArray spanTagText = iterator->bytesTill(endOfTag);
            printf("SPANTAG("); spanTagText.print(); printf(")\n");
            iterator->moveTo(endOfTag);
            return true;
        }

    }

    return false;
}

void VfmdSpanElementsProcessor::processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry)
{
    VfmdSpanTagStack stack;
    VfmdLineArrayIterator iterator = lineArray->begin();
    VfmdLineArrayIterator textFragmentStart;
    assert(textFragmentStart.isValid() == false); // initially invalid

    while (!iterator.isAtEnd()) {
        char currentByte = iterator.nextByte();
        bool isTagIdentifiedAtCurrentPos = false;

        // Ask the span element handlers pertaining to this trigger byte
        int n = registry->spanElementCountForTriggerByte(currentByte);
        for (int i = 0; i < n; i++) {
            VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
            int triggerOptions = registry->triggerOptionsForTriggerByte(currentByte, i);
            isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, triggerOptions,
                                                                              &iterator, &stack, &textFragmentStart);
            if (isTagIdentifiedAtCurrentPos) {
                break;
            }
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

    closeTextFragmentIfOpen(&textFragmentStart, iterator);
}
