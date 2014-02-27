#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdblockelementhandler.h"

VfmdInputLineSequence::VfmdInputLineSequence(const VfmdElementRegistry *registry, const VfmdBlockLineSequence *parentLineSequence)
    : m_registry(registry)
    , m_parentLineSequence(parentLineSequence)
    , m_childLineSequence(0)
    , m_parseTree(0)
{
}

void VfmdInputLineSequence::addLine(const VfmdLine &line) {
    if (!line.isValid()) {
        return;
    }

    if (!m_nextLine.isValid()) {
        m_nextLine = line;
    } else {
        VfmdLine currentLine = m_nextLine;
        m_nextLine = line;
        processInChildSequence(currentLine, m_nextLine);
    }
}

VfmdElementTreeNode* VfmdInputLineSequence::endSequence()
{
    VfmdLine currentLine = m_nextLine;
    m_nextLine = VfmdLine();
    processInChildSequence(currentLine, m_nextLine);
    return m_parseTree;
}

void VfmdInputLineSequence::processInChildSequence(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    // If there's no running child sequence, find and create one
    if (!m_childLineSequence) {
        for (unsigned int i = 0; i < m_registry->blockElementsCount(); i++) {
            VfmdBlockElementHandler *blockHandler = m_registry->blockElementHandler(i);
            blockHandler->createChildSequence(this, currentLine, nextLine);
            if (hasChildSequence()) {
                // the block handler has created and set a child sequence
                break;
            }
        }
    }

    assert(hasChildSequence());

    // Pass the current line on to the child sequence
    m_childLineSequence->processBlockLine(currentLine, nextLine);

    // Check if the child sequence is done
    bool isEndOfLineSequence = nextLine.isInvalid();
    if (isEndOfLineSequence || m_childLineSequence->isEndOfBlock(currentLine, nextLine)) {
        VfmdElementTreeNode *parseSubtree = m_childLineSequence->endBlock();
        if (parseSubtree) {
            if (m_parseTree) {
                m_parseTree->appendSubtreeToEndOfSequence(parseSubtree);
            } else {
                m_parseTree = parseSubtree;
            }
        }
        delete m_childLineSequence;
        m_childLineSequence = 0;
    }
}

const VfmdBlockLineSequence *VfmdInputLineSequence::parentLineSequence() const
{
    return m_parentLineSequence;
}

const VfmdElementRegistry *VfmdInputLineSequence::registry() const
{
    return m_registry;
}

bool VfmdInputLineSequence::hasChildSequence() const
{
    return (m_childLineSequence != 0);
}

void VfmdInputLineSequence::setChildSequence(VfmdBlockLineSequence *lineSequence)
{
    if (m_childLineSequence == 0) {
        m_childLineSequence = lineSequence;
    }
}
