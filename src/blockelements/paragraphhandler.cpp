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

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    VfmdElementTreeNode *paragraphNode = new ParagraphTreeNode();
    VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
    bool ok = paragraphNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    return paragraphNode;
}

ParagraphTreeNode::ParagraphTreeNode()
{
}

ParagraphTreeNode::~ParagraphTreeNode()
{
}
