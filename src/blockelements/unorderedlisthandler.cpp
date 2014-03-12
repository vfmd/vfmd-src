#include "unorderedlisthandler.h"
#include "vfmdcommonregexps.h"
#include "core/vfmdblockutils.h"

void UnorderedListHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdRegexp reStarterPattern = VfmdCommonRegexps::unorderedListStarter();
    if (reStarterPattern.matches(firstLine)) {
        VfmdByteArray listStarterString = reStarterPattern.capturedText(1);
        lineSequence->setChildSequence(new UnorderedListLineSequence(lineSequence, listStarterString));
    }
}

UnorderedListLineSequence::UnorderedListLineSequence(const VfmdInputLineSequence *parent,
                                                     const VfmdByteArray &listStarterString)
    : VfmdBlockLineSequence(parent)
    , m_listStarterString(listStarterString)
    , m_childSequence(0)
    , m_listNode(new UnorderedListTreeNode)
    , m_numOfClosedListItems(0)
{
    m_nextLineStartsWithListStarterString = true;
}

UnorderedListLineSequence::~UnorderedListLineSequence()
{
    delete m_childSequence;
}

void UnorderedListLineSequence::closeListItem(bool isEndOfList)
{
    if (m_childSequence) {
        UnorderedListItemTreeNode *listItemNode = new UnorderedListItemTreeNode();
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

bool UnorderedListLineSequence::isTopPackedListItem(bool isEndOfList) const
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

bool UnorderedListLineSequence::isBottomPackedListItem(bool isEndOfList) const
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

void UnorderedListLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdLine line = currentLine;
    bool isListItemStartLine = m_nextLineStartsWithListStarterString; // is current line starting with m_listStarterString
    if (isListItemStartLine) {
        // current line is the starting line of a list item
        closeListItem(false /* not the end of the list */);
        m_childSequence = new VfmdInputLineSequence(registry(), this);
        m_isCurrentListItemPrecededByABlankLine = m_previousLine.isBlankLine();
        line.chopLeft(m_listStarterString.size());
    } else {
        // current line is not the starting line of a list item
        int numOfPrefixedSpaces = line.indexOfFirstNonSpace();
        if (numOfPrefixedSpaces < 0) {
            // current line is a blank line
            numOfPrefixedSpaces = line.size();
        }
        int prefixedSpacesToRemove = numOfPrefixedSpaces;
        if (prefixedSpacesToRemove > m_listStarterString.size()) {
            prefixedSpacesToRemove = m_listStarterString.size();
        }
        line.chopLeft(prefixedSpacesToRemove);
    }
    m_childSequence->addLine(line);
    m_previousLine = currentLine;
    m_nextLineStartsWithListStarterString = (nextLine.startsWith(m_listStarterString));
}

bool UnorderedListLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    bool currentLineIsABlankLine = currentLine.isBlankLine();

    if (currentLineIsABlankLine && nextLine.isBlankLine()) {
        return true;
    }

    if (!m_nextLineStartsWithListStarterString) {
        int indexOfFirstNonSpace = nextLine.indexOfFirstNonSpace();
        if (indexOfFirstNonSpace < m_listStarterString.size()) { // Should be indented less than a list item
            if (currentLineIsABlankLine) {
                return true;
            } else {
                if (indexOfFirstNonSpace < 4) { // Not a code span
                    if (isHorizontalRuleLine(nextLine)) {
                        return true;
                    }
                    const char firstNonSpaceByte = nextLine.firstNonSpace();
                    if (firstNonSpaceByte == '*' || firstNonSpaceByte == '-' || firstNonSpaceByte == '+') {
                        VfmdRegexp reUnorderedListStarterPattern = VfmdCommonRegexps::unorderedListStarter();
                        if (reUnorderedListStarterPattern.matches(nextLine)) {
                            return true;
                        }
                    }
                    if (firstNonSpaceByte >= '0' && firstNonSpaceByte <= '9') {
                        VfmdRegexp reOrderedListStarterPattern = VfmdCommonRegexps::orderedListStarter();
                        if (reOrderedListStarterPattern.matches(nextLine)) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

VfmdElementTreeNode* UnorderedListLineSequence::endBlock()
{
    closeListItem(true /* end of the list*/);
    return (VfmdElementTreeNode*) m_listNode;
}

void UnorderedListTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write('\n');
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("<ul>\n");
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</ul>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}

void UnorderedListItemTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
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
