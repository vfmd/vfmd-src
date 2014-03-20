#include "setextheaderhandler.h"
#include "vfmdcommonregexps.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdoutputdevice.h"
#include "vfmdelementtreenodestack.h"

void SetextHeaderHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(firstLine);
    if (nextLine == 0) {
        return;
    }
    const char firstUnderlineByte = nextLine->firstByte();
    if ((firstUnderlineByte == '=' || firstUnderlineByte == '-') &&
        (nextLine->matches(VfmdCommonRegexps::setextHeaderUnderline()))) {
        lineSequence->setChildSequence(new SetextHeaderLineSequence(lineSequence));
    }
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

bool SetextHeaderLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    return (m_numOfLinesSeen == 2);
}

VfmdElementTreeNode* SetextHeaderLineSequence::endBlock()
{
    VfmdElementTreeNode *setextNode = new SetextHeaderTreeNode(m_headingLevel);
    VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(m_firstLineContent.trimmed(), registry());
    bool ok = setextNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    return setextNode;
}

SetextHeaderTreeNode::SetextHeaderTreeNode(int headingLevel)
    : m_headingLevel(headingLevel)
{
}

void SetextHeaderTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                      VfmdOutputDevice *outputDevice,
                                      VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write((m_headingLevel == 1)? "<h1>" : "<h2>");
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        outputDevice->write((m_headingLevel == 1)? "</h1>\n" : "</h2>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
