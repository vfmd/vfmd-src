#include <stdio.h>
#include "emphasishandler.h"
#include "vfmdspantagstack.h"

EmphasisHandler::EmphasisHandler()
{
}

static int vfmd_min(int a, int b)
{
    if (a < b) {
        return a;
    }
    return b;
}

static int fringeRankForCategory(VfmdUnicodeProperties::GeneralCategory category)
{
    switch(category) {
    case VfmdUnicodeProperties::ucp_Zs:
    case VfmdUnicodeProperties::ucp_Zl:
    case VfmdUnicodeProperties::ucp_Zp:
    case VfmdUnicodeProperties::ucp_Cc:
    case VfmdUnicodeProperties::ucp_Cf:
        return 0;
    case VfmdUnicodeProperties::ucp_Pc:
    case VfmdUnicodeProperties::ucp_Pd:
    case VfmdUnicodeProperties::ucp_Ps:
    case VfmdUnicodeProperties::ucp_Pe:
    case VfmdUnicodeProperties::ucp_Pi:
    case VfmdUnicodeProperties::ucp_Pf:
    case VfmdUnicodeProperties::ucp_Po:
    case VfmdUnicodeProperties::ucp_Sc:
    case VfmdUnicodeProperties::ucp_Sk:
    case VfmdUnicodeProperties::ucp_Sm:
    case VfmdUnicodeProperties::ucp_So:
        return 1;
    default:
        return 2;
    }
}

static void handlePossibleOpenEmphasisTag(const VfmdByteArray &emphTagString, VfmdSpanTagStack *stack)
{
    assert(emphTagString.size() > 0);
    char firstByte = emphTagString.byteAt(0);
    assert(firstByte == '*' || firstByte == '_');
    stack->push(new OpeningEmphasisTagStackNode(firstByte, emphTagString.size()));
}

static void handlePossibleCloseEmphasisTag(const VfmdByteArray &emphTagString, VfmdSpanTagStack *stack)
{
    assert(emphTagString.size() > 0);
    char firstByte = emphTagString.byteAt(0);
    assert(firstByte == '*' || firstByte == '_');
    int remainingTagStringLength = (int) emphTagString.size();
    while (remainingTagStringLength > 0) {

        VfmdConstants::VfmdOpeningSpanTagStackNodeType stackNodeType = VfmdConstants::UNDEFINED_STACK_NODE;
        if (firstByte == '*') {
            stackNodeType = VfmdConstants::ASTERISK_EMPHASIS_STACK_NODE;
        } else if (firstByte == '_') {
            stackNodeType = VfmdConstants::UNDERSCORE_EMPHASIS_STACK_NODE;
        } else {
            assert(false);
            return;
        }
        int topMostMatchingNodeIndex = stack->indexOfTopmostNodeOfType(stackNodeType);

        if (topMostMatchingNodeIndex >= 0) {

            // There exists a matching opening emphasis tag.
            // So, this is indeed a close emphasis tag.
            stack->popNodesAboveIndexAsTextFragments(topMostMatchingNodeIndex);
            OpeningEmphasisTagStackNode *emphStackNode = dynamic_cast<OpeningEmphasisTagStackNode *>(stack->topNode());
            assert(emphStackNode != 0);
            assert(emphStackNode->character == firstByte);
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
                assert(poppedNode->type() == stackNodeType);
                delete poppedNode;
            }
            stack->topNode()->appendToContainedElements(emphNode);

        } else {

            // No matching opening emphasis tag.
            // So, this is a text fragment.
            stack->topNode()->appendToContainedElements(emphTagString.right(remainingTagStringLength));
            remainingTagStringLength = 0;

        }

    } // end of while(remainingTagStringLength > 0)
}

int EmphasisHandler::identifySpanTagStartingAt(const VfmdByteArray &text, int currentPos, VfmdSpanTagStack *stack) const
{
    const char *data_ptr = text.data();
    unsigned int i = currentPos;
    for (i = currentPos; i < text.size(); i++) {
        const char c = data_ptr[i];
        if (c != '*' && c != '_') {
            break;
        }
    }
    unsigned int lengthOfEmphIndicatorString = (i - currentPos);

    if (lengthOfEmphIndicatorString == 0) {
        // If no '*' or '_' character is found, then this is not an emphasis tag
        return 0;
    }
    assert(!text.isEscapedAtPosition(currentPos));

    VfmdByteArray emphIndicatorString = text.mid(currentPos, lengthOfEmphIndicatorString);
    int beforeEmphIndicatorPos = text.previousUTF8CharStartsAt(currentPos);
    int afterEmphIndicatorPos = currentPos + lengthOfEmphIndicatorString;
    int leftFringeRank  = ((beforeEmphIndicatorPos < 0)?
                               0 : fringeRankForCategory(text.categoryOfUTF8CharStartingAt(beforeEmphIndicatorPos)));
    int rightFringeRank = ((afterEmphIndicatorPos >= text.size())?
                               0 : fringeRankForCategory(text.categoryOfUTF8CharStartingAt(afterEmphIndicatorPos)));

    if (leftFringeRank == rightFringeRank) {
        // Non-flanking. So, not an emphasis tag.
        // Append the text to the partially formed tree.
        stack->topNode()->appendToContainedElements(emphIndicatorString);
        return emphIndicatorString.size();
    }

    int sz = (int) emphIndicatorString.size();
    assert(sz > 0);
    if (sz > 0) {
        int emphTagStartPos = 0;
        for (int i = 1; i < sz; i++) {
            if (emphIndicatorString.byteAt(i) != emphIndicatorString.byteAt(i - 1)) {
                VfmdByteArray emphTagString = emphIndicatorString.mid(emphTagStartPos, i - emphTagStartPos);
                emphTagStartPos = i;
                if (leftFringeRank < rightFringeRank) {
                    // Left-flanking
                    handlePossibleOpenEmphasisTag(emphTagString, stack);
                } else {
                    // Right-flanking
                    handlePossibleCloseEmphasisTag(emphTagString, stack);
                }
            }
        }
        VfmdByteArray lastEmphTagString = emphIndicatorString.mid(emphTagStartPos, sz - emphTagStartPos);
        if (leftFringeRank < rightFringeRank) {
            // Left-flanking
            handlePossibleOpenEmphasisTag(lastEmphTagString, stack);
        } else {
            // Right-flanking
            handlePossibleCloseEmphasisTag(lastEmphTagString, stack);
        }
    }

    return emphIndicatorString.size();
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
