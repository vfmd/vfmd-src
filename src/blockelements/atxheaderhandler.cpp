#include "atxheaderhandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdspanelementsprocessor.h"

bool AtxHeaderHandler::isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph)
{
    UNUSED_ARG(containingBlockType);
    UNUSED_ARG(isAbuttingParagraph);
    assert(isAbuttingParagraph == false);
    return (currentLine->firstByte() == '#');
}

void AtxHeaderHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    AtxHeaderLineSequence *s = new AtxHeaderLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
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
