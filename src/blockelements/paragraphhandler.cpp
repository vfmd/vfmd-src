#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"

void ParagraphHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    ParagraphLineSequence *paragraphLineSequence = new ParagraphLineSequence(lineSequence);
    lineSequence->setChildSequence(paragraphLineSequence);
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
    if (m_isAtEnd) {
        VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
    }
}

bool ParagraphLineSequence::isAtEnd() const
{
    return m_isAtEnd;
}
