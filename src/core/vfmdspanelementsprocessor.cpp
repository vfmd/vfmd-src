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
        VfmdByteArray textFragment = (*textFragmentStart)->bytesTill(iterator);
        printf("TEXTFRAG("); textFragment.print(); printf(")\n");
        textFragmentStart->free();
    }
}

static bool applySpanHandlerOnLineArrayIterator(VfmdSpanElementHandler *spanHandler, VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack, VfmdScopedPointer<VfmdLineArrayIterator> *textFragmentStart)
{
    VfmdLineArrayIterator *endOfTag = iterator->copy();
    spanHandler->processSpanTag(endOfTag, stack);
    if (endOfTag->isAfter(iterator)) { // span tag identified
        closeTextFragmentIfOpen(textFragmentStart, iterator);
        VfmdByteArray spanTagText = iterator->bytesTill(endOfTag);
        printf("SPANTAG("); spanTagText.print(); printf(")\n");
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
    VfmdScopedPointer<VfmdLineArrayIterator> iterator(lineArray->begin());
    VfmdScopedPointer<VfmdLineArrayIterator> textFragmentStart(0);

    while (!iterator->isAtEnd()) {
        char currentByte = iterator->nextByte();
        bool isTagIdentifiedAtCurrentPos = false;

        // Ask the span element handlers pertaining to this trigger byte
        int n = registry->spanElementCountForTriggerByte(currentByte);
        for (int i = 0; i < n; i++) {
            VfmdSpanElementHandler *spanHandler = registry->spanElementForTriggerByte(currentByte, i);
            isTagIdentifiedAtCurrentPos = applySpanHandlerOnLineArrayIterator(spanHandler, iterator.data(), &stack, &textFragmentStart);
            if (isTagIdentifiedAtCurrentPos) {
                break;
            }
        }

        // TODO: Handle auto-link-type span element handlers

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
