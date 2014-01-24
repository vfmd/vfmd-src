#include <stdio.h>
#include "blockquotehandler.h"
#include "vfmdinputlinesequence.h"

void BlockquoteHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    if (lineSequence->currentLine().startsWith("> ")) {
        BlockquoteLineSequence *blockquoteLineSequence = new BlockquoteLineSequence(lineSequence);
        lineSequence->setChildSequence(blockquoteLineSequence);
    }
}

BlockquoteLineSequence::BlockquoteLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
    m_childSequence = new VfmdInputLineSequence(registry());
}

BlockquoteLineSequence::~BlockquoteLineSequence()
{
    delete m_childSequence;
}

void BlockquoteLineSequence::processBlockLine(const VfmdLine &currentLine)
{
    VfmdLine processedLine = currentLine;
    if (processedLine.startsWith("> ")) {
        processedLine.chopLeft(2);
    }
    m_childSequence->addLine(processedLine);
}

bool BlockquoteLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    if (!nextLine.isValid()) {
        return true;
    } else {
        if (currentLine.isBlankLine()) {
            if (nextLine.isBlankLine()) {
                return true;
            }
            if (nextLine.startsWith("    ")) {
                return true;
            }
            if (nextLine.firstNonSpace() != '>') {
                return true;
            }
        } else {
            // TODO: Match with horizontal rule regexp
        }
    }
    return false;
}

VfmdElementTreeNode* BlockquoteLineSequence::endBlock()
{
    VfmdElementTreeNode *blockquoteNode = new BlockquoteTreeNode();
    VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
    blockquoteNode->setChildSubtree(childSubTree);
    return blockquoteNode;
}

BlockquoteTreeNode::BlockquoteTreeNode()
{
}

BlockquoteTreeNode::~BlockquoteTreeNode()
{
}
