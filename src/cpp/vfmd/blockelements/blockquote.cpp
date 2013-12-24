#include <stdio.h>
#include "blockquote.h"
#include "vfmdinputlinesequence.h"

VfmdBlockLineSequence *BlockquoteSyntaxHandler::createBlockLineSequence(const VfmdInputLineSequence *lineSequence)
{
    if (lineSequence->currentLine().startsWith("> ")) {
        return (new BlockquoteLineSequence(lineSequence));
    }
    return 0;
}

BlockquoteLineSequence::BlockquoteLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_isAtEnd(false)
{
    m_childSequence = new VfmdInputLineSequence;
    printf("BEGIN BQ\n");
}

BlockquoteLineSequence::~BlockquoteLineSequence()
{
    delete m_childSequence;
}

void BlockquoteLineSequence::processLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_PARAMETER(nextLine);
    VfmdLine processedLine = currentLine;
    if (processedLine.startsWith("> ")) {
        processedLine.chopLeft(2);
    }
    m_childSequence->addLine(processedLine);

    if (!nextLine.isValid()) {
        m_isAtEnd = true;
    } else {
        if (currentLine.isBlankLine()) {
            if (nextLine.isBlankLine()) {
                m_isAtEnd = true;
            }
            if (nextLine.startsWith("    ")) {
                m_isAtEnd = true;
            }
            if (nextLine.firstNonSpace() != '>') {
                m_isAtEnd = true;
            }
        } else {
            // TODO: Match with horizontal rule regexp
        }
    }

    if (m_isAtEnd) {
        m_childSequence->endSequence();
        printf("END BQ\n");
    }
}

bool BlockquoteLineSequence::isAtEnd() const
{
    return m_isAtEnd;
}
