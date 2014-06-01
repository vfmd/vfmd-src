#include "unorderedlisthandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdlistutils.h"
#include "vfmdelementtreenodestack.h"

bool UnorderedListHandler::isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph)
{
    assert(currentLine->firstNonSpace() == '*' ||
           currentLine->firstNonSpace() == '-' ||
           currentLine->firstNonSpace() == '+');
    if ((!isAbuttingParagraph) ||
        (containingBlockType == VfmdConstants::UNORDERED_LIST_ELEMENT) ||
        (containingBlockType == VfmdConstants::ORDERED_LIST_ELEMENT)) {
        VfmdRegexp reStarterPattern = VfmdCommonRegexps::unorderedListStarter();
        if (reStarterPattern.matches(currentLine->content())) {
            m_listStarterString = reStarterPattern.capturedText(1);
            return true;
        }
    }
    return false;
}

void UnorderedListHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    UnorderedListLineSequence *s = new UnorderedListLineSequence(parentLineSequence,
                                                                 m_listStarterString);
    parentLineSequence->setChildSequence(s);
}

UnorderedListLineSequence::UnorderedListLineSequence(const VfmdInputLineSequence *parent,
                                                     const VfmdByteArray &listStarterString)
    : VfmdBlockLineSequence(parent)
    , m_listStarterString(listStarterString)
    , m_childSequence(0)
    , m_listNode(new UnorderedListTreeNode)
    , m_numOfClosedListItems(0)
    , m_previousLine(0)
{
    m_nextLineStartsWithListStarterString = true;
}

UnorderedListLineSequence::~UnorderedListLineSequence()
{
    delete m_childSequence;
}

void UnorderedListLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    VfmdLine *line = currentLine->copy();
    bool isListItemStartLine = m_nextLineStartsWithListStarterString; // is current line starting with m_listStarterString

    if (isListItemStartLine) { // current line is the starting line of a list item

        // Close the previous list item and add to the parse tree
        if (m_childSequence) {
            bool isFirstListItem = (m_numOfClosedListItems == 0);
            bool isLastListItem = false;
            const VfmdLine *lastLineOfListItem = m_previousLine;
            UnorderedListItemTreeNode *listItemNode = closeListItem<UnorderedListItemTreeNode>(
                        &m_childSequence, isFirstListItem, isLastListItem,
                        lastLineOfListItem, m_isCurrentListItemPrecededByABlankLine);
            m_listNode->adoptAsLastChild(listItemNode);
            m_numOfClosedListItems++;
        }
        assert(m_childSequence == 0); // closeListItem() should have zeroed it

        // Start a new list item
        m_childSequence = new VfmdInputLineSequence(registry(), this);
        m_isCurrentListItemPrecededByABlankLine = (m_previousLine && m_previousLine->isBlankLine());
        line->chopLeft(m_listStarterString.size());

    } else {

        // current line is not the starting line of a list item
        int prefixedSpacesToRemove = line->leadingSpacesCount();
        if (prefixedSpacesToRemove > m_listStarterString.size()) {
            prefixedSpacesToRemove = m_listStarterString.size();
        }
        line->chopLeft(prefixedSpacesToRemove);

    }

    m_childSequence->addLine(line);
    m_previousLine = currentLine->copy();
    m_nextLineStartsWithListStarterString = (nextLine == 0? false : (nextLine->content().startsWith(m_listStarterString)));
}

bool UnorderedListLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    bool currentLineIsABlankLine = currentLine->isBlankLine();

    if (currentLineIsABlankLine && nextLine->isBlankLine()) {
        return true;
    }

    if (!m_nextLineStartsWithListStarterString) {
        int indexOfFirstNonSpace = nextLine->leadingSpacesCount();
        if (indexOfFirstNonSpace < m_listStarterString.size()) { // Should be indented less than a list item
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
                    if ((firstNonSpaceByte >= '0' && firstNonSpaceByte <= '9') &&
                        nextLine->matches(VfmdCommonRegexps::orderedListStarter())) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void UnorderedListLineSequence::endBlock()
{
    // Close the last list item and add to the parse tree
    if (m_childSequence) {
        bool isFirstListItem = (m_numOfClosedListItems == 0);
        bool isLastListItem = true;
        const VfmdLine *lastLineOfListItem = m_previousLine;
        UnorderedListItemTreeNode *listItemNode = closeListItem<UnorderedListItemTreeNode>(
                    &m_childSequence, isFirstListItem, isLastListItem,
                    lastLineOfListItem, m_isCurrentListItemPrecededByABlankLine);
        m_listNode->adoptAsLastChild(listItemNode);
        m_numOfClosedListItems++;
    }
    assert(m_childSequence == 0); // closeListItem() should have zeroed it

    // The parse tree for the list is ready
    setBlockParseTree(m_listNode);
}

void UnorderedListTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write('\n');
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("<ul>\n", 5);
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</ul>\n", 6);
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
