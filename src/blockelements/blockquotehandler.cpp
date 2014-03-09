#include <stdio.h>
#include <assert.h>
#include "blockquotehandler.h"
#include "vfmdinputlinesequence.h"
#include "vfmdcommonregexps.h"
#include "core/vfmdblockutils.h"

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
    m_childSequence = new VfmdInputLineSequence(registry(), this);
}

BlockquoteLineSequence::~BlockquoteLineSequence()
{
    delete m_childSequence;
}

void BlockquoteLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    bool isEndOfParentLineSequence = nextLine.isInvalid();
    if (currentLine.isBlankLine()) {
        // If the last line is a blank line, ignore it
        if (isEndOfParentLineSequence) {
            return;
        }
        if (isEndOfBlock(currentLine, nextLine)) {
            return;
        }
    }

    int lengthOfMatch = numOfBlockquotePrefixBytes(currentLine);
    if (lengthOfMatch > 0) {
        VfmdLine line = currentLine;
        line.chopLeft(lengthOfMatch);
        m_childSequence->addLine(line);
    } else {
        m_childSequence->addLine(currentLine);
    }
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
        if (numOfBlockquotePrefixBytes(nextLine) == 0) {
            return true;
        }
    } else {
        // current line is not a blank line
        if ((!nextLine.startsWith("    ")) && (isHorizontalRuleLine(nextLine))) {
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
