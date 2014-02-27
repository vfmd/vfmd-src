#include "unorderedlisthandler.h"
#include "vfmdcommonregexps.h"

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
    bool isListItemStartLine = (currentLine.startsWith(m_listStarterString));
    if (isListItemStartLine) {
        // current line is the starting line of a list item
        closeListItem(false /* not the end of the list */);
        m_childSequence = new VfmdInputLineSequence(registry());
        m_childSequence->setContainingBlockSequenceType(VfmdConstants::UNORDERED_LIST_ELEMENT);
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
}

bool UnorderedListLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    if (currentLine.isBlankLine()) {
        // current line is a blank line
        if (nextLine.isBlankLine()) {
            return true;
        }
        if (!nextLine.startsWith(m_listStarterString)) {
            VfmdByteArray nextLineStart = nextLine.left(m_listStarterString.size());
            bool nextLineStartHasNonSpace = (nextLineStart.indexOfFirstNonSpace() >= 0);
            if (nextLineStartHasNonSpace) {
                return true;
            }
        }
    } else {
        // current line is not a blank line
        if (!nextLine.startsWith(m_listStarterString)) {
            VfmdByteArray nextLineStart = nextLine.left(m_listStarterString.size());
            bool nextLineStartHasNonSpace = (nextLineStart.indexOfFirstNonSpace() >= 0);
            if (nextLineStartHasNonSpace && !nextLine.startsWith("    ")) {
                VfmdLine nextLineChomped = nextLine;
                nextLineChomped.chomp();
                VfmdRegexp reUnorderedListStarterPattern = VfmdCommonRegexps::unorderedListStarter();
                VfmdRegexp reOrderedListStarterPattern = VfmdCommonRegexps::orderedListStarter();
                VfmdRegexp reHorizontalRulePattern = VfmdCommonRegexps::horizontalRule();
                if (reUnorderedListStarterPattern.matches(nextLineChomped) ||
                    reOrderedListStarterPattern.matches(nextLineChomped) ||
                    reHorizontalRulePattern.matches(nextLineChomped)) {
                    return true;
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
