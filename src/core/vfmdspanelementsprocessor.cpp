#include <stdio.h>
#include "vfmdspanelementsprocessor.h"
#include "vfmdlinearray.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdelementregistry.h"
#include "vfmdspantagstack.h"
#include "vfmdscopedpointer.h"
#include "spanelements/vfmdspanelementhandler.h"

static void closeTextFragmentIfOpen(VfmdScopedPointer<VfmdLineArrayIterator> *textFragmentStart, VfmdLineArrayIterator *iterator)
{
    if (!textFragmentStart->isNull()) {
        if ((*textFragmentStart)->isBefore(iterator)) {
            VfmdByteArray textFragment = (*textFragmentStart)->bytesTill(iterator);
            printf("TEXTFRAG("); textFragment.print(); printf(")\n");
        }
        textFragmentStart->free();
    }
}

static bool applySpanHandlerOnLineArrayIterator(VfmdSpanElementHandler *spanHandler, int triggerOptions,
                                                VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack,
                                                VfmdScopedPointer<VfmdLineArrayIterator> *textFragmentStart)
{
    bool shouldInvokeBeforeTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE) ==
                                          VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE);
    bool shouldInvokeAtTriggerByte = ((triggerOptions & VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE) ==
                                      VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);

    if (shouldInvokeBeforeTriggerByte) {

        VfmdScopedPointer<VfmdLineArrayIterator> fromIter(0);
        if (textFragmentStart->isNull()) {
            fromIter.reset(iterator->copy());
        } else {
            fromIter.reset((*textFragmentStart)->copy());
        }
        VfmdScopedPointer<VfmdLineArrayIterator> toIter(iterator->copy());
        if (!toIter->isAtEnd()) {
            toIter->moveForward(1); // move past the trigger byte
        }
        bool identified = spanHandler->identifySpanTagStartingBetween(fromIter.data(), toIter.data(), stack);
        assert(!(*textFragmentStart)->isAfter(fromIter.data()));
        assert(!iterator->isAfter(toIter.data()));
        assert(fromIter->isBefore(toIter.data()));
        if (
                ((*textFragmentStart)->isAfter(fromIter.data())) ||
                (iterator->isAfter(toIter.data())) ||
                (!fromIter->isBefore(toIter.data()))
                ) {
            return false;
        }
        if (identified) {
            closeTextFragmentIfOpen(textFragmentStart, fromIter.data());
            VfmdByteArray spanTagText = fromIter->bytesTill(toIter.data());
            printf("SPANTAG("); spanTagText.print(); printf(")\n");
            iterator->moveTo(toIter.data());
            return true;
        }

    } else if (shouldInvokeAtTriggerByte) {

        VfmdScopedPointer<VfmdLineArrayIterator> endOfTag(iterator->copy());
        spanHandler->identifySpanTagStartingAt(endOfTag.data(), stack);
        bool identified = endOfTag->isAfter(iterator);
        if (identified) {
            closeTextFragmentIfOpen(textFragmentStart, iterator);
            VfmdByteArray spanTagText = iterator->bytesTill(endOfTag.data());
            printf("SPANTAG("); spanTagText.print(); printf(")\n");
            iterator->moveTo(endOfTag.data());
            return true;
        }

    }

    return false;
}

void VfmdSpanElementsProcessor::processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry)
{
    VfmdSpanTagStack stack;
    VfmdScopedPointer<VfmdLineArrayIterator> iterator(lineArray->begin());
    VfmdScopedPointer<VfmdLineArrayIterator> textFragmentStart(0);

    while (!iterator->isAtEnd()) {
        char currentByte = iterator->nextByte();
        bool isTagIdentifiedAtCurrentPos = false;

        // Ask the span element handlers pertaining to this trigger byte
        int n = registry->spanElementCountForTriggerByte(currentByte);
        for (int i = 0; i < n; i++) {
            VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
            int triggerOptions = registry->triggerOptionsForTriggerByte(currentByte, i);
            isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, triggerOptions,
                                                                              iterator.data(), &stack, &textFragmentStart);
            if (isTagIdentifiedAtCurrentPos) {
                break;
            }
        }

        // All span element handlers rejected this position => text fragment
        if (!isTagIdentifiedAtCurrentPos) {
            if (textFragmentStart.isNull()) { // If no text fragment is open
                // Open a text fragment
                textFragmentStart.reset(iterator->copy());
            }
            assert(!textFragmentStart.isNull());
            iterator->moveForward(1);
        }
    }

    closeTextFragmentIfOpen(&textFragmentStart, iterator.data());
}
