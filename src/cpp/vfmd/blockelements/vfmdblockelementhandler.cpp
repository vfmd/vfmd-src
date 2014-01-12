#include "vfmdblockelementhandler.h"
#include "vfmdinputlinesequence.h"

VfmdBlockLineSequence *VfmdBlockElementHandler::createBlockLineSequence(const VfmdInputLineSequence *lineSequence)
{
    UNUSED_PARAMETER(lineSequence);
    return 0;
}

VfmdBlockElementHandler::~VfmdBlockElementHandler()
{
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
