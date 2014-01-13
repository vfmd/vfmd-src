#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "blockelements/vfmdblockelementhandler.h"

VfmdInputLineSequence::VfmdInputLineSequence(const VfmdElementRegistry *registry)
    : m_registry(registry)
    , m_childLineSequence(0)
{
    m_isAtEnd = false;
}

void VfmdInputLineSequence::addLine(const VfmdLine &line) {
    if (!line.isValid()) {
        return;
    }
    // Store the line locally
    if (!m_currentLine.isValid()) {
        m_currentLine = line;
    } else if (!m_nextLine.isValid()) {
        m_nextLine = line;
    } else {
        m_currentLine = m_nextLine;
        m_nextLine = line;
    }

    if (m_nextLine.isValid()) {
        processLineInChildSequence();
    }
}

void VfmdInputLineSequence::endSequence() {
    m_currentLine = m_nextLine;
    m_nextLine = VfmdLine();
    m_isAtEnd = true;
    processLineInChildSequence();
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
            VfmdBlockLineSequence *blockLineSequence = blockHandler->createBlockLineSequence(this);
            if (blockLineSequence) {
                m_childLineSequence = blockLineSequence;
                break;
            }
        }
    }

    assert(m_childLineSequence != 0);

    // Pass the current line on to the child sequence
    m_childLineSequence->processLine(m_currentLine, m_nextLine);

    // Check if the child sequence is done
    if (isAtEnd() || m_childLineSequence->isAtEnd()) {
        // integrate m_childLineSequence->parseTree();
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
