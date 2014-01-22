#include <assert.h>
#include "vfmdblockelementhandler.h"
#include "vfmdinputlinesequence.h"

void VfmdBlockElementHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    UNUSED_PARAMETER(lineSequence);
}

VfmdBlockElementHandler::~VfmdBlockElementHandler()
{
}

VfmdBlockLineSequence::VfmdBlockLineSequence(const VfmdInputLineSequence *parent)
    : m_parentLineSequence(parent)
{
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

const VfmdInputLineSequence *VfmdBlockLineSequence::parentLineSequence() const
{
    return m_parentLineSequence;
}

const VfmdElementRegistry *VfmdBlockLineSequence::registry() const
{
    assert(m_parentLineSequence != 0);
    return m_parentLineSequence->registry();
}
