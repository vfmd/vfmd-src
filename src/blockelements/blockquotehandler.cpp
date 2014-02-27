#include <stdio.h>
#include <assert.h>
#include "blockquotehandler.h"
#include "vfmdinputlinesequence.h"
#include "vfmdcommonregexps.h"

void BlockquoteHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(nextLine);
    if (firstLine.firstNonSpace() == '>') {
        lineSequence->setChildSequence(new BlockquoteLineSequence(lineSequence));
    }
}

BlockquoteLineSequence::BlockquoteLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
    m_childSequence = new VfmdInputLineSequence(registry());
    m_childSequence->setContainingBlockSequenceType(VfmdConstants::BLOCKQUOTE_ELEMENT);
}

BlockquoteLineSequence::~BlockquoteLineSequence()
{
    delete m_childSequence;
}

void BlockquoteLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    if (currentLine.isBlankLine()) {
        // If the last line is a blank line, ignore it
        if (isEndOfParentLineSequence) {
            return;
        }
        VfmdLine nextLine = parentLineSequence()->nextLine();
        if (isEndOfBlock(currentLine, nextLine)) {
            return;
        }
    }

    VfmdRegexp rePrefixWithEndingSpace = VfmdCommonRegexps::blockquotePrefixWithEndingSpace();
    VfmdRegexp rePrefixWithoutEndingSpace = VfmdCommonRegexps::blockquotePrefixWithoutEndingSpace();

    VfmdLine line = currentLine;
    unsigned int lengthOfMatch = 0;
    if (rePrefixWithEndingSpace.matches(line)) {
        lengthOfMatch = rePrefixWithEndingSpace.capturedText(0).size();
    } else if (rePrefixWithoutEndingSpace.matches(line)) {
        lengthOfMatch = rePrefixWithoutEndingSpace.capturedText(0).size();
    }
    if (lengthOfMatch > 0) {
        line.chopLeft(lengthOfMatch);
    }
    m_childSequence->addLine(line);
}

bool BlockquoteLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    if (!nextLine.isValid()) {
        return true;
    }
    if (currentLine.isBlankLine()) {
        // current line is a blank line
        if (nextLine.isBlankLine()) {
            return true;
        }
        if (nextLine.startsWith("    ")) {
            return true;
        }
        if (nextLine.firstNonSpace() != '>') {
            return true;
        }
    } else {
        // current line is not a blank line
        VfmdRegexp reHorizontalRule = VfmdCommonRegexps::horizontalRule();
        if ((!nextLine.startsWith("    ")) &&
            (reHorizontalRule.matches(nextLine.chomped())) ) {
            return true;
        }
    }
    return false;
}

VfmdElementTreeNode* BlockquoteLineSequence::endBlock()
{
    VfmdElementTreeNode *blockquoteNode = new BlockquoteTreeNode();
    VfmdElementTreeNode *childSubTree = m_childSequence->endSequence();
    bool ok = blockquoteNode->setChildNodeIfNotSet(childSubTree);
    assert(ok);
    return blockquoteNode;
}

BlockquoteTreeNode::BlockquoteTreeNode()
{
}

BlockquoteTreeNode::~BlockquoteTreeNode()
{
}

void BlockquoteTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                   VfmdOutputDevice *outputDevice,
                                   VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("<blockquote>\n");
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("</blockquote>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
