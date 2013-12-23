#include <stdio.h>
#include "paragraph.h"
#include "vfmdinputlinesequence.h"

VfmdBlockLineSequence *ParagraphSyntaxHandler::createBlockLineSequence(const VfmdInputLineSequence *lineSequence)
{
    return (new ParagraphLineSequence(lineSequence));
}

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_isAtEnd(false)
{
}

void ParagraphLineSequence::processLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_PARAMETER(nextLine);
    currentLine.print("PARA LINE");
    if (currentLine.isBlankLine() || !nextLine.isValid()) {
        printf("END OF PARA\n");
        m_isAtEnd = true;
    }
}

bool ParagraphLineSequence::isAtEnd() const
{
    return m_isAtEnd;
}
