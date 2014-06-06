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
    : VfmdBlockLineSequence(parent)
    , m_numOfBytesExcludingLastSeenBlankLine(0)
{
}

void CodeBlockLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    VfmdByteArray lineContent = currentLine->content();
    int indexOfFirstNonSpace = currentLine->leadingSpacesCount();
    if (indexOfFirstNonSpace >= 4) {
        lineContent.chopLeft(4);
    }
    if (m_content.isInvalid()) {
        m_content = lineContent;
    } else {
        m_content.append(lineContent);
    }
    m_content.appendByte('\n');
    if (!currentLine->isBlankLine()) {
        m_numOfBytesExcludingLastSeenBlankLine = m_content.size();
    }
}

bool CodeBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    return ((nextLine == 0) ||
            (nextLine->isBlankLine() && currentLine->isBlankLine()) ||
            (!nextLine->isBlankLine() && (nextLine->leadingSpacesCount() < 4))
            );
}

void CodeBlockLineSequence::endBlock()
{
    setBlockParseTree(new CodeBlockTreeNode(content()));
}

VfmdByteArray CodeBlockLineSequence::content() const
{
    return m_content.left(m_numOfBytesExcludingLastSeenBlankLine);
}

CodeBlockTreeNode::CodeBlockTreeNode(const VfmdByteArray &content)
    : m_content(content)
{
}
