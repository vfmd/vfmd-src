#include "orderedlisthandler.h"
#include "core/vfmdcommonregexps.h"
#include "vfmdelementtreenodestack.h"

bool OrderedListHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine,
                                        int containingBlockType, bool isAbuttingParagraph)
{
    UNUSED_ARG(nextLine);
    if ((!isAbuttingParagraph) ||
        (containingBlockType == VfmdConstants::UNORDERED_LIST_ELEMENT) ||
        (containingBlockType == VfmdConstants::ORDERED_LIST_ELEMENT)) {
        char firstNonSpaceByte = currentLine->firstNonSpace();
        if ((firstNonSpaceByte >= '0') && (firstNonSpaceByte <= '9')) {
            VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();
            if (reStarterPattern.matches(currentLine->content())) {
                m_listStarterString = reStarterPattern.capturedText(1);
                m_startingNumber = reStarterPattern.capturedText(2);
                return true;
            }
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

void OrderedListLineSequence::closeListItem(bool isEndOfList)
{
    if (m_childSequence) {
        OrderedListItemTreeNode *listItemNode = new OrderedListItemTreeNode();
        listItemNode->setTopPacked(isTopPackedListItem(isEndOfList));
        listItemNode->setBottomPacked(isBottomPackedListItem(isEndOfList));
        VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
        bool ok = listItemNode->setChildNodeIfNotSet(childSubTree);
        assert(ok);
        m_listNode->adoptAsLastChild(listItemNode);
        delete m_childSequence;
        m_childSequence = 0;
        m_numOfClosedListItems++;
    }
}

bool OrderedListLineSequence::isTopPackedListItem(bool isEndOfList) const
{
    // This method is assumed to be called only from closeListItem()
    const VfmdLine *lastLineOfListItem = m_previousLine;
    bool isFirstListItem = (m_numOfClosedListItems == 0);
    bool isLastListItem = isEndOfList;
    bool isTopPacked = false;
    if (isFirstListItem && isLastListItem) {
        isTopPacked = true;
    } else if (isFirstListItem && !lastLineOfListItem->isBlankLine()) {
        isTopPacked = true;
    } else if (!isFirstListItem && !m_isCurrentListItemPrecededByABlankLine) {
        isTopPacked = true;
    }
    return isTopPacked;
}

bool OrderedListLineSequence::isBottomPackedListItem(bool isEndOfList) const
{
    // This method is assumed to be called only from closeListItem()
    const VfmdLine *lastLineOfListItem = m_previousLine;
    bool isFirstListItem = (m_numOfClosedListItems == 0);
    bool isLastListItem = isEndOfList;
    bool isBottomPacked = false;
    if (isFirstListItem && isLastListItem) {
        isBottomPacked = true;
    } else if (!lastLineOfListItem->isBlankLine()) {
        isBottomPacked = true;
    } else if (isLastListItem && !m_isCurrentListItemPrecededByABlankLine) {
        isBottomPacked = true;
    }
    return isBottomPacked;
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

    if (currentLineStartPrefixLength > 0) {
        // current line is the starting line of a list item
        closeListItem(false /* not the end of the list */);
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
    closeListItem(true /* end of the list*/);
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

void OrderedListTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write('\n');
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        if (m_startingNumber.size() > 0 && !m_startingNumber.isEqualTo("1")) {
            outputDevice->write("<ol start=\"", 11);
            outputDevice->write(m_startingNumber);
            outputDevice->write("\">\n", 3);
        } else {
            outputDevice->write("<ol>\n", 5);
        }
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</ol>\n", 6);
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}

void OrderedListItemTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        bool containsASingleParagraph = (firstChildNode()->elementType() == VfmdConstants::PARAGRAPH_ELEMENT &&
                                         firstChildNode()->nextNode() == 0);
        if (containsASingleParagraph) {
            outputDevice->write("<li>", 4);
        } else {
            outputDevice->write("<li>\n", 5);
        }
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if (!containsASingleParagraph) {
            if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
                renderHtmlIndent(outputDevice, ancestorNodes);
            }
        }
        outputDevice->write("</li>\n", 6);
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
