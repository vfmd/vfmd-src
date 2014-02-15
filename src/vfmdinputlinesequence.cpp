#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdblockelementhandler.h"

VfmdInputLineSequence::VfmdInputLineSequence(const VfmdElementRegistry *registry)
    : m_registry(registry)
    , m_childLineSequence(0)
    , m_parseTree(0)
{
    m_isAtEnd = false;
}

void VfmdInputLineSequence::addLine(const VfmdLine &line) {
    if (!line.isValid()) {
        return;
    }

    if (!m_nextLine.isValid()) {
        m_nextLine = line;
    } else {
        m_currentLine = m_nextLine;
        m_nextLine = line;
        processLineInChildSequence();
    }
}

VfmdElementTreeNode* VfmdInputLineSequence::endSequence()
{
    m_currentLine = m_nextLine;
    m_nextLine = VfmdLine();
    m_isAtEnd = true;
    processLineInChildSequence();
    return m_parseTree;
}

bool VfmdInputLineSequence::isAtEnd() const {
    return m_isAtEnd;
}

void VfmdInputLineSequence::processLineInChildSequence()
{
    // If there's no running child sequence, find and create one
    if (!m_childLineSequence) {
        for (unsigned int i = 0; i < m_registry->blockElementsCount(); i++) {
            VfmdBlockElementHandler *blockHandler = m_registry->blockElementHandler(i);
            blockHandler->createChildSequence(this);
            if (hasChildSequence()) {
                // the block handler has created and set a child sequence
                break;
            }
        }
    }

    assert(hasChildSequence());

    // Pass the current line on to the child sequence
    m_childLineSequence->processBlockLine(m_currentLine);

    // Check if the child sequence is done
    if (isAtEnd() || m_childLineSequence->isEndOfBlock(m_currentLine, m_nextLine)) {
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

VfmdLine VfmdInputLineSequence::currentLine() const
{
    return m_currentLine;
}

VfmdLine VfmdInputLineSequence::nextLine() const
{
    return m_nextLine;
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
