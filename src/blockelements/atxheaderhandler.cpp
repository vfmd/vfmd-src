#include "atxheaderhandler.h"
#include "core/vfmdcommonregexps.h"
#include "vfmdlinearray.h"
#include "vfmdspanelementsprocessor.h"

void AtxHeaderHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    VfmdLine firstLine = lineSequence->currentLine();
    if ((firstLine.size() > 0) &&
        (firstLine.byteAt(0) == '#')) {
        lineSequence->setChildSequence(new AtxHeaderLineSequence(lineSequence));
    }
}

AtxHeaderLineSequence::AtxHeaderLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

void AtxHeaderLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    UNUSED_ARG(isEndOfParentLineSequence);
    m_headerLine = currentLine;
    m_headerLine.chomp(); // Remove trailing newline
}

bool AtxHeaderLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    // There can be only one line
    return true;
}

VfmdElementTreeNode* AtxHeaderLineSequence::endBlock()
{
    VfmdRegexp reLineWithHeaderText = VfmdCommonRegexps::atxHeaderLineWithHeaderText();
    VfmdRegexp reLineWithoutHeaderText = VfmdCommonRegexps::atxHeaderLineWithoutHeaderText();
    int headingLevel = 0;
    VfmdLine headerContent;
    if (reLineWithHeaderText.matches(m_headerLine)) {
        headingLevel = reLineWithHeaderText.capturedText(1).size();
        int numOfTrailingHashes = reLineWithHeaderText.capturedText(2).size();
        headerContent = m_headerLine;
        headerContent.chopLeft(headingLevel);
        headerContent.chopRight(numOfTrailingHashes);
        headerContent.trim();
    } else if (reLineWithoutHeaderText.matches(m_headerLine)) {
        headingLevel = reLineWithoutHeaderText.capturedText(1).size();
    }
    assert(headingLevel > 0);
    if (headingLevel > 6) {
        headingLevel = 6;
    }

    VfmdElementTreeNode *atxNode = new AtxHeaderTreeNode(headingLevel);
    if (headerContent.size() > 0) {
        VfmdLineArray lineArray;
        lineArray.addLine(headerContent);
        lineArray.trim();
        VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(&lineArray, registry());
        bool ok = atxNode->setChildNodeIfNotSet(spanParseTree);
        assert(ok);
    }
    return atxNode;
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
            outputDevice->write("<h");
            outputDevice->write('0' + m_headingLevel);
            outputDevice->write('>');
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            outputDevice->write("</h");
            outputDevice->write('0' + m_headingLevel);
            outputDevice->write(">\n");
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}