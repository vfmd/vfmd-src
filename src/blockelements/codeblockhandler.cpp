#include "codeblockhandler.h"
#include "core/htmltextrenderer.h"

bool CodeBlockHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    return (currentLine->leadingSpacesCount() >= 4);
}

void CodeBlockHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    CodeBlockLineSequence *s = new CodeBlockLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
}

CodeBlockLineSequence::CodeBlockLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_isAtEnd(false)
{
}

void CodeBlockLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    bool isEndOfParentLineSequence = (nextLine == 0);
    assert(currentLine->isBlankLine() || (currentLine->leadingSpacesCount() >= 4));
    if (isEndOfParentLineSequence) {
        m_isAtEnd = true;
    } else {
        if (!nextLine->isBlankLine() && nextLine->leadingSpacesCount() < 4) {
            m_isAtEnd = true;
        }
    }
    VfmdByteArray lineContent = currentLine->content();
    if (m_isAtEnd && currentLine->isBlankLine()) {
        // Don't let the last line be a blank line
        return;
    }
    int indexOfFirstNonSpace = currentLine->leadingSpacesCount();
    if (indexOfFirstNonSpace >= 4) {
        lineContent.chopLeft(4);
    }
    m_content.append(lineContent);
    m_content.appendByte('\n');
}

bool CodeBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    return m_isAtEnd;
}

void CodeBlockLineSequence::endBlock()
{
    setBlockParseTree(new CodeBlockTreeNode(m_content));
}

VfmdByteArray CodeBlockLineSequence::content() const
{
    return m_content;
}

CodeBlockTreeNode::CodeBlockTreeNode(const VfmdByteArray &content)
    : m_content(content)
{
}

void CodeBlockTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write("<pre><code>", 11);
        HtmlTextRenderer::renderCode(outputDevice, m_content);
        outputDevice->write("</code></pre>\n", 14);
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
