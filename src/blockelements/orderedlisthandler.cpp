#include "orderedlisthandler.h"
#include "vfmdcommonregexps.h"

void OrderedListHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    VfmdLine firstLine = lineSequence->currentLine();
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
{
}

OrderedListLineSequence::~OrderedListLineSequence()
{
    delete m_childSequence;
}

void OrderedListLineSequence::closeListItem()
{
    if (m_childSequence) {
        VfmdElementTreeNode *listItemNode = new OrderedListItemTreeNode();
        VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
        bool ok = listItemNode->setChildNodeIfNotSet(childSubTree);
        assert(ok);
        m_listNode->adoptAsLastChild(listItemNode);
        delete m_childSequence;
        m_childSequence = 0;
    }
}

void OrderedListLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    UNUSED_ARG(isEndOfParentLineSequence);
    VfmdLine line = currentLine;
    VfmdRegexp reStarterPattern = VfmdCommonRegexps::orderedListStarter();

    VfmdByteArray lineStart = currentLine.left(m_listStarterStringLength);
    bool lineStartHasNonSpace = (lineStart.indexOfFirstNonSpace() >= 0);

    bool isListItemStartLine = (reStarterPattern.matches(currentLine) && lineStartHasNonSpace);
    if (isListItemStartLine) {
        // current line is the starting line of a list item
        closeListItem();
        m_childSequence = new VfmdInputLineSequence(registry());
        m_childSequence->setContainingBlockSequenceType(VfmdConstants::ORDERED_LIST_ELEMENT);
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
                VfmdLine nextLineChomped = nextLine.chomped();
                VfmdRegexp reUnorderedListStarterPattern = VfmdCommonRegexps::unorderedListStarter();
                VfmdRegexp reHorizontalRulePattern = VfmdCommonRegexps::horizontalRule();
                if (reUnorderedListStarterPattern.matches(nextLineChomped) ||
                    reHorizontalRulePattern.matches(nextLineChomped)) {
                    return true;
                }
            }
        }
    }
    return false;
}

VfmdElementTreeNode* OrderedListLineSequence::endBlock()
{
    closeListItem();
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
