#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdblockelementhandler.h"

VfmdInputLineSequence::VfmdInputLineSequence(const VfmdElementRegistry *registry, const VfmdBlockLineSequence *parentLineSequence)
    : m_registry(registry)
    , m_parentLineSequence(parentLineSequence)
    , m_childLineSequence(0)
    , m_numOfLinesGivenToChildLineSequence(0)
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
        if (m_childLineSequence) {
            m_numOfLinesGivenToChildLineSequence = 0;
        }
    }

    assert(hasChildSequence());

    // Pass the current line on to the child sequence
    m_childLineSequence->processBlockLine(currentLine, nextLine);
    m_numOfLinesGivenToChildLineSequence++;

    // If necessary, end the child sequence
    bool isEndOfLineSequence = nextLine.isInvalid();
    if (isEndOfLineSequence || m_childLineSequence->isEndOfBlock(currentLine, nextLine)) {
        // End the child block
        VfmdElementTreeNode *parseSubtree = m_childLineSequence->endBlock();
        VfmdPointerArray<const VfmdLine> *unconsumedLines = m_childLineSequence->linesSinceEndOfBlock();
        if (parseSubtree) {
            if (m_parseTree) {
                m_parseTree->appendSubtreeToEndOfSequence(parseSubtree);
            } else {
                m_parseTree = parseSubtree;
            }
        }
        delete m_childLineSequence;
        m_childLineSequence = 0;

        // Re-process any unconsumed lines
        if (unconsumedLines && (unconsumedLines->size() > 0)) {
            unsigned int sz = unconsumedLines->size();
            assert(unconsumedLines->lastItem()->isEqualTo(currentLine));
            assert(sz < m_numOfLinesGivenToChildLineSequence); // A block cannot disown all lines passed to it
            if (unconsumedLines->lastItem()->isEqualTo(currentLine) &&
                (sz < m_numOfLinesGivenToChildLineSequence)) {
                for (unsigned int i = 0; i < (sz - 1); i++) {
                    const VfmdLine *line = unconsumedLines->itemAt(i);
                    const VfmdLine *nextLine = unconsumedLines->itemAt(i + 1);
                    processInChildSequence(*line, *nextLine);
                }
                processInChildSequence(currentLine, nextLine);
            }
        }
        if (unconsumedLines) {
            unconsumedLines->freeItemsAndClear();
            delete unconsumedLines;
        }
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
