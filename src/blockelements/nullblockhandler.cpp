#include "nullblockhandler.h"
#include "core/vfmdcommonregexps.h"
#include "vfmdlinearray.h"
#include "vfmdspanelementsprocessor.h"

void NullBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    if (firstLine.isBlankLine()) {
        NullBlockLineSequence *nullBlockLineSequence = new NullBlockLineSequence(lineSequence);
        lineSequence->setChildSequence(nullBlockLineSequence);
    }
}

NullBlockLineSequence::NullBlockLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

void NullBlockLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
}

bool NullBlockLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(currentLine);
    return (!nextLine.isBlankLine());
}

VfmdElementTreeNode* NullBlockLineSequence::endBlock() {
    return 0;
}
