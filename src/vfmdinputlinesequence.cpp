#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdblockelementhandler.h"
#include "blockelements/paragraphhandler.h"

VfmdInputLineSequence::VfmdInputLineSequence(const VfmdElementRegistry *registry, const VfmdBlockLineSequence *parentLineSequence)
    : m_parentLineSequence(parentLineSequence)
    , m_containingBlockType(parentLineSequence? parentLineSequence->elementType() : VfmdConstants::UNDEFINED_BLOCK_ELEMENT)
    , m_registry(registry)
    , m_nextLine(0)
    , m_childLineSequence(0)
    , m_paragraphLineSequence(0)
    , m_numOfLinesGivenToChildLineSequence(0)
    , m_nextBlockHandler(0)
    , m_parseTree(0)
{
}

void VfmdInputLineSequence::addLine(VfmdLine *line)
{
    assert(line != 0);

    if (m_nextLine == 0) {
        m_nextLine = line;
    } else {
        processInChildSequence(m_nextLine, line);
        delete m_nextLine;
        m_nextLine = line;
    }
}

VfmdElementTreeNode* VfmdInputLineSequence::endSequence()
{
    processInChildSequence(m_nextLine, 0);
    delete m_nextLine;
    m_nextLine = 0;
    return m_parseTree;
}

void VfmdInputLineSequence::processInChildSequence(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    // If there's no running child sequence, find and create one
    if (!m_childLineSequence) {
        VfmdBlockElementHandler *selectedBlockHandler = 0;
        if (m_nextBlockHandler) {
            selectedBlockHandler = m_nextBlockHandler;
        } else {
            for (unsigned int i = 0; i < m_registry->blockElementsCount(); i++) {
                VfmdBlockElementHandler *blockHandler = m_registry->blockElementHandler(i);
                bool found = blockHandler->isStartOfBlock(currentLine, nextLine, m_containingBlockType, false);
                if (found) {
                    selectedBlockHandler = blockHandler;
                    break;
                }
            }
        }
        if (selectedBlockHandler) {
            selectedBlockHandler->createLineSequence(this);
        } else {
            // If no block handler comes forward to start a block in this line,
            // start a paragraph.
            m_paragraphLineSequence = new ParagraphLineSequence(this);
            m_childLineSequence = m_paragraphLineSequence;
        }
        assert(m_childLineSequence != 0);
        m_numOfLinesGivenToChildLineSequence = 0;
    }

    assert(hasChildSequence());

    // Pass the current line on to the child sequence
    m_childLineSequence->processBlockLine(currentLine, nextLine);
    m_numOfLinesGivenToChildLineSequence++;

    // If necessary, end the child sequence
    bool isEndOfLineSequence = (nextLine == 0);
    if (isEndOfLineSequence || m_childLineSequence->isEndOfBlock(currentLine, nextLine)) {

        // End the child block
        m_childLineSequence->endBlock();
        VfmdPointerArray<const VfmdLine> *unconsumedLines = 0;
        m_nextBlockHandler = 0;
        if (m_paragraphLineSequence) {
            m_nextBlockHandler = m_paragraphLineSequence->nextBlockHandler();
            unconsumedLines = m_paragraphLineSequence->linesSinceEndOfParagraph();
            m_paragraphLineSequence = 0;
        }
        delete m_childLineSequence;
        m_childLineSequence = 0;

        // Re-process any unconsumed lines
        if (unconsumedLines && (unconsumedLines->size() > 0)) {
            unsigned int sz = unconsumedLines->size();
            assert(unconsumedLines->lastItem()->content() == currentLine->content());
            assert(sz < m_numOfLinesGivenToChildLineSequence); // A block cannot disown all lines passed to it
            if ((unconsumedLines->lastItem()->content() == currentLine->content()) &&
                (sz < m_numOfLinesGivenToChildLineSequence)) {
                const VfmdLine *nextUnconsumedLine = unconsumedLines->itemAt(0);
                for (unsigned int i = 1; i < sz; i++) {
                    const VfmdLine *unconsumedLine = unconsumedLines->itemAt(i);
                    processInChildSequence(nextUnconsumedLine, unconsumedLine);
                    delete nextUnconsumedLine;
                    nextUnconsumedLine = unconsumedLine;
                }
                processInChildSequence(currentLine, nextLine);
            }
        }
        if (unconsumedLines) {
            unconsumedLines->clear();
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

int VfmdInputLineSequence::containingBlockType() const
{
    return m_containingBlockType;
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

void VfmdInputLineSequence::addToParseTree(VfmdElementTreeNode *subtree)
{
    if (subtree) {
        if (m_parseTree) {
            m_parseTree->appendSubtreeToEndOfSequence(subtree);
        } else {
            m_parseTree = subtree;
        }
    }
}
