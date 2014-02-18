#include "unorderedlisthandler.h"
#include "vfmdcommonregexps.h"

void UnorderedListHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    VfmdLine firstLine = lineSequence->currentLine();
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
{
}

UnorderedListLineSequence::~UnorderedListLineSequence()
{
    delete m_childSequence;
}

void UnorderedListLineSequence::closeListItem()
{
    if (m_childSequence) {
        VfmdElementTreeNode *listItemNode = new UnorderedListItemTreeNode();
        VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
        bool ok = listItemNode->setChildNodeIfNotSet(childSubTree);
        assert(ok);
        m_listNode->adoptAsLastChild(listItemNode);
        delete m_childSequence;
        m_childSequence = 0;
    }
}

void UnorderedListLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    UNUSED_ARG(isEndOfParentLineSequence);
    VfmdLine line = currentLine;
    bool isListItemStartLine = (currentLine.startsWith(m_listStarterString));
    if (isListItemStartLine) {
        // current line is the starting line of a list item
        closeListItem();
        m_childSequence = new VfmdInputLineSequence(registry());
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
                VfmdLine nextLineChomped = nextLine.chomped();
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
    closeListItem();
    return (VfmdElementTreeNode*) m_listNode;
}

void UnorderedListTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
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
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</li>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
