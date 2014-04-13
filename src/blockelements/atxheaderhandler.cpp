#include "atxheaderhandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdspanelementsprocessor.h"

void AtxHeaderHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine->firstByte() == '#') {
        lineSequence->setChildSequence(new AtxHeaderLineSequence(lineSequence));
    }
}

AtxHeaderLineSequence::AtxHeaderLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

void AtxHeaderLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(nextLine);
    m_headerLineText = currentLine->content();
}

bool AtxHeaderLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    // There can be only one line
    return true;
}

void AtxHeaderLineSequence::endBlock()
{
    VfmdRegexp reLineWithHeaderText = VfmdCommonRegexps::atxHeaderLineWithHeaderText();
    VfmdRegexp reLineWithoutHeaderText = VfmdCommonRegexps::atxHeaderLineWithoutHeaderText();
    int headingLevel = 0;
    VfmdByteArray headerContent;
    if (reLineWithHeaderText.matches(m_headerLineText)) {
        headingLevel = reLineWithHeaderText.capturedText(1).size();
        int numOfTrailingHashes = reLineWithHeaderText.capturedText(2).size();
        headerContent = m_headerLineText;
        headerContent.chopLeft(headingLevel);
        headerContent.chopRight(numOfTrailingHashes);
        headerContent.trim();
    } else if (reLineWithoutHeaderText.matches(m_headerLineText)) {
        headingLevel = reLineWithoutHeaderText.capturedText(1).size();
    }
    assert(headingLevel > 0);
    if (headingLevel > 6) {
        headingLevel = 6;
    }

    VfmdElementTreeNode *atxNode = new AtxHeaderTreeNode(headingLevel);
    if (headerContent.size() > 0) {
        headerContent.trim();
        VfmdSpanElementsProcessor spanElementsProcessor(headerContent, registry());
        VfmdElementTreeNode *spanParseTree = spanElementsProcessor.parseTree();
        bool ok = atxNode->setChildNodeIfNotSet(spanParseTree);
        assert(ok);
    }
    setBlockParseTree(atxNode);
}

AtxHeaderTreeNode::AtxHeaderTreeNode(int headingLevel)
    : m_headingLevel(headingLevel)
{
}

void AtxHeaderTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        assert(m_headingLevel >= 1);
        assert(m_headingLevel <= 6);
        if (m_headingLevel >= 1 && m_headingLevel <= 6) {
            outputDevice->write("<h", 2);
            outputDevice->write('0' + m_headingLevel);
            outputDevice->write('>');
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            outputDevice->write("</h", 3);
            outputDevice->write('0' + m_headingLevel);
            outputDevice->write(">\n");
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
