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

static int vfmd_min(int a, int b)
{
    if (a < b) {
        return a;
    }
    return b;
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
        return;
    } else if (rightFlankedBySpace) {
        // Can be closing emphasis tags or text fragments, depending on what's on the stack
        unsigned int remainingTagStringLength = numberOfAsterisks;
        while (remainingTagStringLength > 0) {
            int topMostMatchingNodeIndex = stack->indexOfTopmostNodeOfType(VfmdConstants::ASTERISK_EMPHASIS_STACK_NODE);
            if (topMostMatchingNodeIndex >= 0) {
                // A closing emphasis tag
                OpeningEmphasisTagStackNode *emphStackNode = dynamic_cast<OpeningEmphasisTagStackNode *>(stack->nodeAt(topMostMatchingNodeIndex));
                assert(emphStackNode != 0);
                assert(emphStackNode->character == '*');
                stack->popNodesAboveIndexAsTextFragments(topMostMatchingNodeIndex);
                EmphasisTreeNode *emphNode = new EmphasisTreeNode(emphStackNode->character,
                                                                  vfmd_min(remainingTagStringLength, emphStackNode->repetitionCount));
                emphNode->adoptContainedElements(emphStackNode);

                if (emphStackNode->repetitionCount > remainingTagStringLength) {
                    // The open emph is not fully closed. Retain the node in the stack.
                    emphStackNode->repetitionCount -= remainingTagStringLength;
                    remainingTagStringLength = 0;
                } else {
                    // The open emph is fully closed. Remove from the stack.
                    remainingTagStringLength -= emphStackNode->repetitionCount;
                    VfmdOpeningSpanTagStackNode *poppedNode = stack->pop();
                    assert(poppedNode != 0);
                    assert(poppedNode->type() == VfmdConstants::ASTERISK_EMPHASIS_STACK_NODE);
                    delete poppedNode;
                }

                stack->topNode()->appendToContainedElements(emphNode);

            } else {
                // A text fragment
                VfmdByteArray ba;
                ba.reserve(remainingTagStringLength);
                while (remainingTagStringLength--) {
                    ba.appendByte('*');
                }
                stack->topNode()->appendToContainedElements(ba);
                break;
            }
        }
        return;
    }

    // Not an emph tag. Move the iterator back to the original position.
    iterator->moveTo(startOfTag);
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

void OpeningEmphasisTagStackNode::populateEquivalentText(VfmdByteArray *ba) const
{
    ba->clear();
    ba->reserve(repetitionCount);
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
