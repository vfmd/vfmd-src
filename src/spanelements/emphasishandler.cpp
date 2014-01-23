#include <stdio.h>
#include "emphasishandler.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdspantagstack.h"

EmphasisHandler::EmphasisHandler()
{
}

EmphasisHandler::~EmphasisHandler()
{
}

void EmphasisHandler::processSpanTag(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const
{
    UNUSED_PARAMETER(stack);
    VfmdLineArrayIterator *startOfTag = iterator->copy();
    char nextByte = iterator->nextByte();
    if (nextByte != '*') {
        return;
    }
    iterator->moveForwardOverBytesInString("*");
    unsigned int numberOfAsterisks = startOfTag->numberOfBytesTill(iterator);
    bool leftFlankedBySpace = (startOfTag->previousByte() == ' ');
    bool rightFlankedBySpace = (iterator->nextByte() == ' ');
    if (leftFlankedBySpace) {
        // Can only be an opening emphasis tag
        stack->push(new VfmdSpanTagStackNode(VfmdSpanTagStackNode::ASTERISK_EMPHASIS, '*', numberOfAsterisks));
        printf("Opening emphasis: %c x %d\n", '*', numberOfAsterisks);
    } else if (rightFlankedBySpace) {
        VfmdSpanTagStackNode *topMostEmphNode = stack->topmostNodeOfType(VfmdSpanTagStackNode::ASTERISK_EMPHASIS);
        if (topMostEmphNode) {
            stack->popNodesAbove(topMostEmphNode);
            if (topMostEmphNode->character == '*') {
                if (topMostEmphNode->repetitionCount == numberOfAsterisks) {
                    printf("Closing emphasis: %c x %d\n", topMostEmphNode->character, topMostEmphNode->repetitionCount);
                } else if (topMostEmphNode->repetitionCount > numberOfAsterisks) {
                    printf("Closing emphasis: %c x %d\n", topMostEmphNode->character, numberOfAsterisks);
                    topMostEmphNode->repetitionCount -= numberOfAsterisks;
                } else if (topMostEmphNode->repetitionCount < numberOfAsterisks) {
                    printf("Closing emphasis: %c x %d\n", topMostEmphNode->character, numberOfAsterisks);
                }
            }
        } else {
            // No emph tag is open, so this is a text fragment
            VfmdByteArray ba = startOfTag->bytesTill(iterator);
            ba.print("Text frag:");
            printf("\n");
        }
    } else {
        // Not an emph tag. Move the iterator back to the original position.
        iterator->moveTo(startOfTag);
    }
    delete startOfTag;
}
