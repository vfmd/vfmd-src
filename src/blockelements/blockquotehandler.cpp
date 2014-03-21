#include <stdio.h>
#include <assert.h>
#include "blockquotehandler.h"
#include "vfmdinputlinesequence.h"
#include "vfmdcommonregexps.h"
#include "core/vfmdblockutils.h"

void BlockquoteHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine->firstNonSpace() == '>') {
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

void BlockquoteLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    // Check if we're at the end of the blockquote
    bool isEndOfBlockquote = false;
    if (nextLine == 0) {
        isEndOfBlockquote = true;
    } else {
        if (currentLine->isBlankLine()) {
            if (nextLine->isBlankLine() ||
                (nextLine->leadingSpacesCount() >= 4) ||
                (nextLine->firstNonSpace() != '>')) {
                isEndOfBlockquote = true;
            }
        } else {
            if ((nextLine->leadingSpacesCount() < 4) &&
                isHorizontalRuleLine(nextLine)) {
                isEndOfBlockquote = true;
            }
        }
    }
    m_isAtEndOfBlockquote = isEndOfBlockquote;

    // Process the line
    if (m_isAtEndOfBlockquote && currentLine->isBlankLine()) {
        // If the last line is a blank line, ignore it
        return;
    }
    VfmdLine *line = currentLine->copy();
    if (currentLine->firstNonSpace() == '>') {
        line->chopLeft(currentLine->leadingSpacesCount() + 1);
        if (line->firstByte() == ' ') {
            line->chopLeft(1);
        }
    }
    m_childSequence->addLine(line);
}

bool BlockquoteLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return m_isAtEndOfBlockquote;
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
