#include "nullblockhandler.h"
#include "core/vfmdcommonregexps.h"

bool NullBlockHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    return (currentLine->isBlankLine());
}

void NullBlockHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    NullBlockLineSequence *s = new NullBlockLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
}

NullBlockLineSequence::NullBlockLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

void NullBlockLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
}

bool NullBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    return (!nextLine->isBlankLine());
}

void NullBlockLineSequence::endBlock()
{
    // Nothing to do
}
