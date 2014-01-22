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
    : VfmdBlockLineSequence(parent)
{
}

void ParagraphLineSequence::processBlockLine(const VfmdLine &currentLine)
{
    m_lineArray.addLine(currentLine);
}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    return (currentLine.isBlankLine() || !nextLine.isValid());
}

void ParagraphLineSequence::endBlock()
{
    printf("PARAGRAPH (\n");
    m_lineArray.print();
    printf(")\n");
    VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
}
