#include <assert.h>
#include "vfmdinputlinesequence.h"
#include "blockelements/paragraph.h"
#include "blockelements/blockquote.h"

VfmdInputLineSequence::VfmdInputLineSequence()
    : m_childLineSequence(0)
{
    m_blockSyntaxHandlers[0] = new BlockquoteSyntaxHandler;
    m_blockSyntaxHandlers[1] = new ParagraphSyntaxHandler;
    m_blockSyntaxHandlerCount = 2;
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
        for (int i = 0; i < m_blockSyntaxHandlerCount; i++) {
            VfmdBlockLineSequence *blockLineSequence = m_blockSyntaxHandlers[i]->createBlockLineSequence(this);
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
