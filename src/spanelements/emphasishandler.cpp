#include <stdio.h>
#include "emphasishandler.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdspantagstack.h"
#include "vfmdscopedpointer.h"

EmphasisHandler::EmphasisHandler()
{
}

EmphasisHandler::~EmphasisHandler()
{
}

void EmphasisHandler::identifySpanTagStartingAt(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const
{
    VfmdLineArrayIterator startOfTag = (*iterator);
    char nextByte = iterator->nextByte();
    if (nextByte != '*') {
        return;
    }
    iterator->moveForwardOverBytesInString("*");
    unsigned int numberOfAsterisks = startOfTag.numberOfBytesTill(*iterator);
    bool leftFlankedBySpace = (startOfTag.previousByte() == ' ');
    bool rightFlankedBySpace = (iterator->nextByte() == ' ');
    if (leftFlankedBySpace) {
        // Can only be an opening emphasis tag
        stack->push(new OpeningEmphasisTagStackNode('*', numberOfAsterisks));
        printf("Opening emphasis: %c x %d\n", '*', numberOfAsterisks);
    } else if (rightFlankedBySpace) {
        // Check if it can be a closing emphasis tag
        // TODO
        /*
        OpeningEmphasisTagStackNode *topMostEmphNode = 0;
        VfmdOpeningSpanTagStackNode *topMostRelevantNode = stack->topmostNodeOfType(VfmdConstants::ASTERISK_EMPHASIS_STACK_NODE);
        if (topMostRelevantNode) {
            topMostEmphNode = dynamic_cast<OpeningEmphasisTagStackNode *>(topMostRelevantNode);
        }
        if (topMostEmphNode) {
            stack->popNodesAboveAsTextFragments(topMostRelevantNode);
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
            VfmdByteArray ba = startOfTag.bytesTill(*iterator);
            ba.print("Text frag:");
            printf("\n");
        }
        */
    } else {
        // Not an emph tag. Move the iterator back to the original position.
        iterator->moveTo(startOfTag);
    }
}

OpeningEmphasisTagStackNode::OpeningEmphasisTagStackNode(char c, int n)
    : VfmdOpeningSpanTagStackNode()
    , character(c)
    , repetitionCount(n)
{
    assert(c == '*' || c == '_');
}

OpeningEmphasisTagStackNode::~OpeningEmphasisTagStackNode()
{
}

int OpeningEmphasisTagStackNode::type() const
{
    if (character == '*') {
        return VfmdConstants::ASTERISK_EMPHASIS_STACK_NODE;
    } else if (character == '_') {
        return VfmdConstants::UNDERSCORE_EMPHASIS_STACK_NODE;
    }
    return VfmdConstants::UNDEFINED_STACK_NODE;
}

void OpeningEmphasisTagStackNode::appendEquivalentTextToByteArray(VfmdByteArray *ba)
{
    for (unsigned int i = 0; i < repetitionCount; i++) {
        ba->appendByte(character);
    }
}

void OpeningEmphasisTagStackNode::print() const
{
    printf("emphasis (\"");
    for (unsigned int i = 0; i < repetitionCount; i++) {
        printf("%c", character);
    }
    printf("\")");
}
