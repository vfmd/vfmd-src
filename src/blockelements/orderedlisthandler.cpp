#include "orderedlisthandler.h"
#include "vfmdcommonregexps.h"
#include "core/vfmdblockutils.h"

void OrderedListHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();
    if (reStarterPattern.matches(firstLine)) {
        VfmdByteArray listStarterString = reStarterPattern.capturedText(1);
        VfmdByteArray startingNumber = reStarterPattern.capturedText(2);
        lineSequence->setChildSequence(new OrderedListLineSequence(lineSequence, listStarterString.size(), startingNumber));
    }
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
{
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
    VfmdLine lastLineOfListItem = m_previousLine;
    bool isFirstListItem = (m_numOfClosedListItems == 0);
    bool isLastListItem = isEndOfList;
    bool isTopPacked = false;
    if (isFirstListItem && isLastListItem) {
        isTopPacked = true;
    } else if (isFirstListItem && !lastLineOfListItem.isBlankLine()) {
        isTopPacked = true;
    } else if (!isFirstListItem && !m_isCurrentListItemPrecededByABlankLine) {
        isTopPacked = true;
    }
    return isTopPacked;
}

bool OrderedListLineSequence::isBottomPackedListItem(bool isEndOfList) const
{
    // This method is assumed to be called only from closeListItem()
    VfmdLine lastLineOfListItem = m_previousLine;
    bool isFirstListItem = (m_numOfClosedListItems == 0);
    bool isLastListItem = isEndOfList;
    bool isBottomPacked = false;
    if (isFirstListItem && isLastListItem) {
        isBottomPacked = true;
    } else if (!lastLineOfListItem.isBlankLine()) {
        isBottomPacked = true;
    } else if (!isLastListItem && !m_isCurrentListItemPrecededByABlankLine) {
        isBottomPacked = true;
    }
    return isBottomPacked;
}

void OrderedListLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdLine line = currentLine;
    VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();

    VfmdByteArray lineStart = currentLine.left(m_listStarterStringLength);
    bool lineStartHasNonSpace = (lineStart.indexOfFirstNonSpace() >= 0);

    bool isListItemStartLine = (reStarterPattern.matches(currentLine) && lineStartHasNonSpace);
    if (isListItemStartLine) {
        // current line is the starting line of a list item
        closeListItem(false /* not the end of the list */);
        m_childSequence = new VfmdInputLineSequence(registry(), this);
        m_isCurrentListItemPrecededByABlankLine = m_previousLine.isBlankLine();
        int matchLength = (int) reStarterPattern.capturedText(1).size();
        line.chopLeft(matchLength);
    } else {
        // current line is not the starting line of a list item
        int numOfPrefixedSpaces = line.indexOfFirstNonSpace();
        if (numOfPrefixedSpaces < 0) {
            // current line is a blank line
            numOfPrefixedSpaces = line.size();
        }
        int prefixedSpacesToRemove = numOfPrefixedSpaces;
        if (prefixedSpacesToRemove > m_listStarterStringLength) {
            prefixedSpacesToRemove = m_listStarterStringLength;
        }
        line.chopLeft(prefixedSpacesToRemove);
    }
    m_childSequence->addLine(line);
    m_previousLine = currentLine;
}

bool OrderedListLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    VfmdRegexp reOrderedListStarterPattern = VfmdCommonRegexps::orderedListStarter();
    if (currentLine.isBlankLine()) {
        // current line is a blank line
        if (nextLine.isBlankLine()) {
            return true;
        }
        if (!reOrderedListStarterPattern.matches(nextLine)) {
            VfmdByteArray nextLineStart = nextLine.left(m_listStarterStringLength);
            bool nextLineStartHasNonSpace = (nextLineStart.indexOfFirstNonSpace() >= 0);
            if (nextLineStartHasNonSpace) {
                return true;
            }
        }
    } else {
        // current line is not a blank line
        if (!reOrderedListStarterPattern.matches(nextLine)) {
            VfmdByteArray nextLineStart = nextLine.left(m_listStarterStringLength);
            bool nextLineStartHasNonSpace = (nextLineStart.indexOfFirstNonSpace() >= 0);
            if (nextLineStartHasNonSpace && !nextLine.startsWith("    ")) {
                VfmdRegexp reUnorderedListStarterPattern = VfmdCommonRegexps::unorderedListStarter();
                if (reUnorderedListStarterPattern.matches(nextLine) ||
                    isHorizontalRuleLine(nextLine)) {
                    return true;
                }
            }
        }
    }
    return false;
}

VfmdElementTreeNode* OrderedListLineSequence::endBlock()
{
    closeListItem(true /* end of the list*/);
    return (VfmdElementTreeNode*) m_listNode;
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
            outputDevice->write("<ol start=\"");
            outputDevice->write(m_startingNumber);
            outputDevice->write("\">\n");
        } else {
            outputDevice->write("<ol>\n");
        }
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</ol>\n");
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
        outputDevice->write("<li>");
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        outputDevice->write("</li>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
