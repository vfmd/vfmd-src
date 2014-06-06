#include <stdio.h>
#include <assert.h>
#include "blockquotehandler.h"
#include "vfmdinputlinesequence.h"
#include "core/vfmdcommonregexps.h"

bool BlockquoteHandler::isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph)
{
    assert(currentLine->firstNonSpace() == '>');
    return ((!isAbuttingParagraph) || (containingBlockType == VfmdConstants::BLOCKQUOTE_ELEMENT));
}

void BlockquoteHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    BlockquoteLineSequence *s = new BlockquoteLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
}

BlockquoteLineSequence::BlockquoteLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
    , m_childSequence(new VfmdInputLineSequence(registry(), this))
    , m_trailingBlankLine(0)
{
}

BlockquoteLineSequence::~BlockquoteLineSequence()
{
    delete m_trailingBlankLine;
    delete m_childSequence;
}

void BlockquoteLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    if (m_trailingBlankLine) {
        // The last-seen blank line is not the last line of the blockquote,
        // and so it will be passed on to the child sequence.
        m_childSequence->addLine(m_trailingBlankLine);
        m_trailingBlankLine = 0;
    }

    VfmdLine *line = currentLine->copy();

    if (line->isBlankLine()) {
        // If the blank line turns out to be the last line of the blockquote,
        // it will not be passed on to the child sequence.
        m_trailingBlankLine = line;
    } else {
        if (currentLine->firstNonSpace() == '>') {
            line->chopLeft(currentLine->leadingSpacesCount() + 1);
            if (line->firstByte() == ' ') {
                line->chopLeft(1);
            }
        }
        m_childSequence->addLine(line);
    }
}

bool BlockquoteLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    if (currentLine->isBlankLine()) {
        return (nextLine->isBlankLine() || nextLine->leadingSpacesCount() >= 4 || nextLine->firstNonSpace() != '>');
    } else {
        return (nextLine->leadingSpacesCount() < 4 && nextLine->isHorizontalRuleLine());
    }
}

void BlockquoteLineSequence::endBlock()
{
    VfmdElementTreeNode *blockquoteNode = new BlockquoteTreeNode();
    VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
    bool ok = blockquoteNode->setChildNodeIfNotSet(childSubTree);
    assert(ok);
    setBlockParseTree(blockquoteNode);
}

BlockquoteTreeNode::BlockquoteTreeNode()
{
}
