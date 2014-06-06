#include "orderedlisthandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdlistutils.h"
#include "vfmdelementtreenodestack.h"

bool OrderedListHandler::isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph)
{
    assert((currentLine->firstNonSpace() >= '0') && (currentLine->firstNonSpace() <= '9'));
    if ((!isAbuttingParagraph) ||
        (containingBlockType == VfmdConstants::UNORDERED_LIST_ELEMENT) ||
        (containingBlockType == VfmdConstants::ORDERED_LIST_ELEMENT)) {
        VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();
        if (reStarterPattern.matches(currentLine->content())) {
            m_listStarterString = reStarterPattern.capturedText(1);
            m_startingNumber = reStarterPattern.capturedText(2);
            return true;
        }
    }
    return false;
}

void OrderedListHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    assert(m_listStarterString.size() > 0);
    assert(m_startingNumber.size() > 0);
    OrderedListLineSequence *s = new OrderedListLineSequence(parentLineSequence,
                                                             m_listStarterString.size(),
                                                             m_startingNumber);
    parentLineSequence->setChildSequence(s);
}

OrderedListLineSequence::OrderedListLineSequence(const VfmdInputLineSequence *parent,
                                                 int listStarterStringLength,
                                                 const VfmdByteArray &startingNumber)
    : VfmdBlockLineSequence(parent)
    , m_listStarterStringLength(listStarterStringLength)
    , m_startingNumber(startingNumber)
    , m_childSequence(0)
    , m_listNode(new OrderedListTreeNode(startingNumber))
    , m_numOfClosedListItems(0)
    , m_previousLine(0)
{
    m_nextLineListItemStartPrefixLength = listStarterStringLength;
}

OrderedListLineSequence::~OrderedListLineSequence()
{
    delete m_childSequence;
}

static unsigned int listItemStartPrefixLength(const VfmdByteArray &lineContent, int listStarterStringLength)
{
    int indexOfFirstNonSpace = lineContent.indexOfFirstNonSpace();
    const char firstNonSpaceByte = lineContent.firstNonSpace();

    if ((indexOfFirstNonSpace < listStarterStringLength) && // Not a sub-list
        (firstNonSpaceByte >= '0') && // Starts with a digit
        (firstNonSpaceByte <= '9')) {
        VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();
        if (reStarterPattern.matches(lineContent)) {
            return reStarterPattern.capturedText(1).size();
        }
    }
    return 0;
}

void OrderedListLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdLine *line = currentLine->copy();

    unsigned int currentLineStartPrefixLength = m_nextLineListItemStartPrefixLength;

    if (currentLineStartPrefixLength > 0) { // current line is the starting line of a list item

        // Close the previous list item and add to the parse tree
        if (m_childSequence) {
            bool isFirstListItem = (m_numOfClosedListItems == 0);
            bool isLastListItem = false;
            const VfmdLine *lastLineOfListItem = m_previousLine;
            OrderedListItemTreeNode *listItemNode = closeListItem<OrderedListItemTreeNode>(
                        &m_childSequence, isFirstListItem, isLastListItem,
                        lastLineOfListItem, m_isCurrentListItemPrecededByABlankLine);
            m_listNode->adoptAsLastChild(listItemNode);
            m_numOfClosedListItems++;
        }
        assert(m_childSequence == 0); // closeListItem() should have zeroed it

        // Start a new list item
        m_childSequence = new VfmdInputLineSequence(registry(), this);
        m_isCurrentListItemPrecededByABlankLine = (m_previousLine && m_previousLine->isBlankLine());
        line->chopLeft(currentLineStartPrefixLength);

    } else {

        // current line is not the starting line of a list item
        int prefixedSpacesToRemove = line->leadingSpacesCount();
        if (prefixedSpacesToRemove > m_listStarterStringLength) {
            prefixedSpacesToRemove = m_listStarterStringLength;
        }
        line->chopLeft(prefixedSpacesToRemove);

    }

    m_childSequence->addLine(line);
    m_previousLine = currentLine->copy();

    m_nextLineListItemStartPrefixLength = (nextLine == 0? 0 : listItemStartPrefixLength(nextLine->content(), m_listStarterStringLength));
}

bool OrderedListLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    bool currentLineIsABlankLine = currentLine->isBlankLine();
    if (currentLineIsABlankLine && nextLine->isBlankLine()) {
        return true;
    }

    bool isListItemStartingAtNextLine = (m_nextLineListItemStartPrefixLength > 0);

    if (!isListItemStartingAtNextLine) {
        int indexOfFirstNonSpace = nextLine->leadingSpacesCount();
        if (indexOfFirstNonSpace < m_listStarterStringLength) { // Should be indented less than a list item
            if (currentLineIsABlankLine) {
                return true;
            } else {
                if (indexOfFirstNonSpace < 4) { // Not a code span
                    if (nextLine->isHorizontalRuleLine()) {
                        return true;
                    }
                    const char firstNonSpaceByte = nextLine->firstNonSpace();
                    if ((firstNonSpaceByte == '*' || firstNonSpaceByte == '-' || firstNonSpaceByte == '+') &&
                        nextLine->matches(VfmdCommonRegexps::unorderedListStarter())) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void OrderedListLineSequence::endBlock()
{
    // Close the last list item and add to the parse tree
    if (m_childSequence) {
        bool isFirstListItem = (m_numOfClosedListItems == 0);
        bool isLastListItem = true;
        const VfmdLine *lastLineOfListItem = m_previousLine;
        OrderedListItemTreeNode *listItemNode = closeListItem<OrderedListItemTreeNode>(
                    &m_childSequence, isFirstListItem, isLastListItem,
                    lastLineOfListItem, m_isCurrentListItemPrecededByABlankLine);
        m_listNode->adoptAsLastChild(listItemNode);
        m_numOfClosedListItems++;
    }
    assert(m_childSequence == 0); // closeListItem() should have zeroed it

    // The parse tree for the list is ready
    setBlockParseTree(m_listNode);
}

OrderedListTreeNode::OrderedListTreeNode(const VfmdByteArray& startingNumber)
    : m_startingNumber(startingNumber)
{
    // Remove leading zeroes in the starting number
    while (m_startingNumber.size() > 0 && m_startingNumber.byteAt(0) == '0') {
        m_startingNumber.chopLeft(1);
    }
}
