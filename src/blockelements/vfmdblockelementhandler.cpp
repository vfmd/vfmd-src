#include <assert.h>
#include "vfmdblockelementhandler.h"
#include "vfmdinputlinesequence.h"

void VfmdBlockElementHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    UNUSED_ARG(lineSequence);
}

const char *VfmdBlockElementHandler::description() const
{
    return "generic-block";
}

VfmdBlockLineSequence::VfmdBlockLineSequence(const VfmdInputLineSequence *parent)
    : m_parentLineSequence(parent)
{
}

VfmdBlockLineSequence::~VfmdBlockLineSequence()
{
}

void VfmdBlockLineSequence::processBlockLine(const VfmdLine &currentLine)
{
    /* Base implementation does nothing */
    UNUSED_ARG(currentLine);
}

bool VfmdBlockLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return false;
}

VfmdElementTreeNode* VfmdBlockLineSequence::endBlock()
{
    /* Base implementation does nothing */
    return 0;
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
