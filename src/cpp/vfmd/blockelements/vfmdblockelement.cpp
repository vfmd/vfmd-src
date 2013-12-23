#include "vfmdblockelement.h"
#include "vfmdinputlinesequence.h"

VfmdBlockLineSequence *VfmdBlockSyntaxHandler::createBlockLineSequence(const VfmdInputLineSequence *lineSequence)
{
    UNUSED_PARAMETER(lineSequence);
    return 0;
}

VfmdBlockLineSequence::VfmdBlockLineSequence(const VfmdInputLineSequence *parent)
{
    UNUSED_PARAMETER(parent);
}

VfmdBlockLineSequence::~VfmdBlockLineSequence()
{
}

void VfmdBlockLineSequence::processLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    /* Base implementation does nothing */
    UNUSED_PARAMETER(currentLine);
    UNUSED_PARAMETER(nextLine);
}

bool VfmdBlockLineSequence::isAtEnd() const
{
    return false;
}