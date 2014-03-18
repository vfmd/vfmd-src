#include "codeblockhandler.h"

void CodeBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine->leadingSpacesCount() >= 4) {
        lineSequence->setChildSequence(new CodeBlockLineSequence(lineSequence));
    }
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
        if (nextLine->leadingSpacesCount() < 4) {
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

bool CodeBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    return m_isAtEnd;
}

VfmdElementTreeNode* CodeBlockLineSequence::endBlock()
{
    m_content.squeeze();
    return new CodeBlockTreeNode(m_content);
}

CodeBlockTreeNode::CodeBlockTreeNode(const VfmdByteArray &content)
    : m_content(content)
{
}

void CodeBlockTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write("<pre><code>");
        const char *data_ptr = m_content.data();
        size_t sz = m_content.size();
        if (data_ptr && sz) {
            for (unsigned int i = 0; i < sz; i++) {
                const char c = data_ptr[i];
                if (c == '<') {
                    outputDevice->write("&lt;");
                } else if (c == '>') {
                    outputDevice->write("&gt;");
                } else if (c == '&') {
                    outputDevice->write("&amp;");
                } else {
                    outputDevice->write(c);
                }
            }
        }
        outputDevice->write("</code></pre>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
