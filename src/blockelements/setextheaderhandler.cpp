#include "setextheaderhandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdspanelementsprocessor.h"
#include "vfmdoutputdevice.h"
#include "vfmdelementtreenodestack.h"

bool SetextHeaderHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    if (nextLine == 0) {
        return false;
    }
    const char firstUnderlineByte = nextLine->firstByte();
    if (firstUnderlineByte == '=' || firstUnderlineByte == '-') {
        VfmdRegexp reSetextHeaderUnderline = VfmdCommonRegexps::setextHeaderUnderline();
        return (nextLine->matches(reSetextHeaderUnderline));
    }
    return false;
}

void SetextHeaderHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    SetextHeaderLineSequence *s = new SetextHeaderLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
}

SetextHeaderLineSequence::SetextHeaderLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_numOfLinesSeen(0), m_headingLevel(0)
{
}

void SetextHeaderLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    if (m_numOfLinesSeen == 0) {
        m_firstLineContent = currentLine->content();
    } else if (m_numOfLinesSeen == 1) {
        char firstByteOfSecondLine = currentLine->firstByte();
        assert(firstByteOfSecondLine == '-' || firstByteOfSecondLine == '=');
        if (firstByteOfSecondLine == '=') {
            m_headingLevel = 1;
        } else {
            m_headingLevel = 2;
        }
    }
    m_numOfLinesSeen++;
}

bool SetextHeaderLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    return (m_numOfLinesSeen == 2);
}

void SetextHeaderLineSequence::endBlock()
{
    VfmdElementTreeNode *setextNode = new SetextHeaderTreeNode(m_headingLevel);
    VfmdSpanElementsProcessor spanElementsProcessor(m_firstLineContent.trimmed(), registry());
    VfmdElementTreeNode *spanParseTree = spanElementsProcessor.parseTree();
    bool ok = setextNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    setBlockParseTree(setextNode);
}

SetextHeaderTreeNode::SetextHeaderTreeNode(int headingLevel)
    : m_headingLevel(headingLevel)
{
}
