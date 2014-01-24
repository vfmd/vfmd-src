#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "vfmdscopedpointer.h"

void ParagraphHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    ParagraphLineSequence *paragraphLineSequence = new ParagraphLineSequence(lineSequence);
    lineSequence->setChildSequence(paragraphLineSequence);
}

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

ParagraphLineSequence::~ParagraphLineSequence()
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

#include "vfmdlinearrayiterator.h"

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    // VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
    // TODO: Process span elements
    VfmdScopedPointer<VfmdLineArrayIterator> start(m_lineArray.begin());
    VfmdScopedPointer<VfmdLineArrayIterator> end(m_lineArray.end());

    VfmdByteArray paragraphText = start->bytesTill(end.data());

    VfmdElementTreeNode *textNode = new TextSpanTreeNode(paragraphText);
    VfmdElementTreeNode *paragraphNode = new ParagraphTreeNode();
    paragraphNode->setChildSubtree(textNode);

    return paragraphNode;
}

ParagraphTreeNode::ParagraphTreeNode()
{
}

ParagraphTreeNode::~ParagraphTreeNode()
{
}
