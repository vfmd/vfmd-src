#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdblockelementhandler.h"
#include "blockelements/paragraphhandler.h"

struct BlockCallbackContext {
    const VfmdLine *currentLine, *nextLine;
    int containingBlockType;
    VfmdInputLineSequence *inputLineSequence;
    bool isUsingTriggerByte;
};

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

bool blockElementsCallback(VfmdBlockElementHandler *handler, int options, void *context)
{
    UNUSED_ARG(options);
    struct BlockCallbackContext *ctx = static_cast<struct BlockCallbackContext *>(context);
    bool found;
    if (ctx->isUsingTriggerByte) {
        found = handler->isStartOfBlock(ctx->currentLine, ctx->containingBlockType, false);
    } else {
        found = handler->isStartOfBlock(ctx->currentLine, ctx->nextLine);
    }
    if (found) {
        ctx->inputLineSequence->setNextChildBlockHandler(handler);
    }
    return found; // If the block handler identified this as the starting line, end the iteration
}

void VfmdInputLineSequence::processInChildSequence(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    // If there's no running child sequence, find and create one
    if (!m_childLineSequence) {

        VfmdBlockElementHandler *selectedBlockHandler = 0;
        if (m_nextBlockHandler) {
            selectedBlockHandler = m_nextBlockHandler;
        }
        if (selectedBlockHandler == 0) { // First query block handlers without any trigger byte
            const unsigned int count = m_registry->numberOfBlockElementsWithoutAnyTriggerByte();
            for (unsigned int i = 0; i < count; i++) {
                VfmdBlockElementHandler *blockHandler = m_registry->blockElementWithoutAnyTriggerByteAtIndex(i);
                if (blockHandler->isStartOfBlock(currentLine, nextLine)) {
                    selectedBlockHandler = blockHandler;
                    break;
                }
            }
        }
        if (selectedBlockHandler == 0) { // Failing that, query block handlers with a trigger byte
            const char triggerByte = currentLine->firstNonSpace();
            const unsigned int count = m_registry->numberOfBlockElementsForTriggerByte(triggerByte);
            for (unsigned int i = 0; i < count; i++) {
                VfmdBlockElementHandler *blockHandler = m_registry->blockElementForTriggerByteAtIndex(triggerByte, i);
                if (blockHandler->isStartOfBlock(currentLine, m_containingBlockType, false)) {
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

void VfmdInputLineSequence::setNextChildBlockHandler(VfmdBlockElementHandler *handler)
{
    m_nextBlockHandler = handler;
}

VfmdBlockElementHandler* VfmdInputLineSequence::nextChildBlockHandler() const
{
    return m_nextBlockHandler;
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
