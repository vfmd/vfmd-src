#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdinputlinesequence.h"

VfmdBlockLineSequence *ParagraphHandler::createBlockLineSequence(const VfmdInputLineSequence *lineSequence)
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
    m_lineArray.addLine(currentLine);
    if (currentLine.isBlankLine() || !nextLine.isValid()) {
        printf("PARAGRAPH (\n");
        m_lineArray.print();
        printf(")\n");
        m_isAtEnd = true;
    }
}

bool ParagraphLineSequence::isAtEnd() const
{
    return m_isAtEnd;
}
