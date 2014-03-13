#include "codeblockhandler.h"

void CodeBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine.startsWith("    ")) {
        lineSequence->setChildSequence(new CodeBlockLineSequence(lineSequence));
    }
}

CodeBlockLineSequence::CodeBlockLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_isAtEnd(false)
{
}

void CodeBlockLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    bool isEndOfParentLineSequence = nextLine.isInvalid();
    assert(currentLine.isBlankLine() || currentLine.startsWith("    "));
    if (isEndOfParentLineSequence) {
        m_isAtEnd = true;
    } else {
        if (!nextLine.startsWith("    ")) {
            m_isAtEnd = true;
        }
    }
    VfmdLine line = currentLine;
    if (m_isAtEnd && line.isBlankLine()) {
        // Don't let the last line be a blank line
        return;
    }
    int indexOfFirstNonSpace = line.indexOfFirstNonSpace();
    assert((indexOfFirstNonSpace < 0)  /* blank line */ ||
           (indexOfFirstNonSpace >= 4) /* starts with 4 spaces */);
    if (indexOfFirstNonSpace >= 4) {
        line.chopLeft(4);
    }
    m_content.append(line);
    m_content.appendByte('\n');
}

bool CodeBlockLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
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
