#include <stdio.h>
#include "vfmdspanelementsprocessor.h"
#include "vfmdlinearray.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdelementregistry.h"
#include "vfmdspantagstack.h"
#include "spanelements/vfmdspanelementhandler.h"

static void closeTextFragmentIfOpen(VfmdLineArrayIterator **textFragmentStart, VfmdLineArrayIterator *iterator)
{
    if ((*textFragmentStart) != 0) {
        VfmdByteArray *textFragment = (*textFragmentStart)->bytesTill(iterator);
        textFragment->print("Text fragment");
        delete textFragment;
        delete (*textFragmentStart);
        (*textFragmentStart) = 0;
    }
}

static bool applySpanHandlerOnLineArrayIterator(VfmdSpanElementHandler *spanHandler, VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack, VfmdLineArrayIterator **textFragmentStart)
{
    VfmdLineArrayIterator *endOfTag = iterator->copy();
    spanHandler->processSpanTag(endOfTag, stack);
    if (endOfTag->isAfter(iterator)) { // span tag identified
        VfmdByteArray *spanTagText = iterator->bytesTill(endOfTag);
        spanTagText->print("Span tag");
        delete spanTagText;
        closeTextFragmentIfOpen(textFragmentStart, iterator);
        iterator->moveTo(endOfTag);
        delete endOfTag;
        return true;
    }
    delete endOfTag;
    return false;
}

void VfmdSpanElementsProcessor::processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry)
{
    VfmdSpanTagStack stack;
    VfmdLineArrayIterator *iterator = lineArray->begin();
    VfmdLineArrayIterator *textFragmentStart = 0;

    while (!iterator->isAtEnd()) {
        char currentByte = iterator->nextByte();
        bool isTagIdentifiedAtCurrentPos = false;

        // Ask the span element handlers pertaining to this trigger byte
        int n = registry->spanElementCountForTriggerByte(currentByte);
        for (int i = 0; i < n; i++) {
            VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
            isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, iterator, &stack, &textFragmentStart);
            if (isTagIdentifiedAtCurrentPos) {
                break;
            }
        }

        // Ask the generic span element handlers
        if (!isTagIdentifiedAtCurrentPos) {
            int n = registry->spanElementsWithoutTriggerByteCount();
            for (int i = 0; i < n; i++) {
                VfmdSpanElementHandler *spanHandler = registry->spanElementWithoutTriggerByte(i);
                isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, iterator, &stack, &textFragmentStart);
                if (isTagIdentifiedAtCurrentPos) {
                    break;
                }
            }
        }

        // All span element handlers rejected this position => text fragment
        if (!isTagIdentifiedAtCurrentPos) {
            if (!textFragmentStart) { // If no text fragment is open
                // Open a text fragment
                textFragmentStart = iterator->copy();
            }
            assert(textFragmentStart != 0);
        }
    }

    closeTextFragmentIfOpen(&textFragmentStart, iterator);
}
