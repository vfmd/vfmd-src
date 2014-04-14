#include <assert.h>
#include "vfmdblockelementhandler.h"
#include "vfmdinputlinesequence.h"

bool VfmdBlockElementHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine,
                                             int containingBlockType, bool isAbuttingParagraph)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    UNUSED_ARG(containingBlockType);
    UNUSED_ARG(isAbuttingParagraph);
    return false;
}

void VfmdBlockElementHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    UNUSED_ARG(parentLineSequence);
    /* Base implementation does nothing */
}

const char *VfmdBlockElementHandler::description() const
{
    return "generic-block";
}

VfmdBlockLineSequence::VfmdBlockLineSequence(const VfmdInputLineSequence *parent)
{
    // Removing const for being able to add to the parent's parse tree
    m_parentLineSequence = const_cast<VfmdInputLineSequence *>(parent);
}

VfmdBlockLineSequence::~VfmdBlockLineSequence()
{
}

int VfmdBlockLineSequence::elementType() const
{
    return VfmdConstants::UNDEFINED_BLOCK_ELEMENT;
}

void VfmdBlockLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    /* Base implementation does nothing */
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
}

bool VfmdBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return false;
}

void VfmdBlockLineSequence::endBlock()
{
    /* Base implementation does nothing */
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

void VfmdBlockLineSequence::setBlockParseTree(VfmdElementTreeNode *subtree)
{
    m_parentLineSequence->addToParseTree(subtree);
}
