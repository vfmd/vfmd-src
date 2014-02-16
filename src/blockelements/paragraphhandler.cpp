#include <stdio.h>
#include "paragraphhandler.h"
#include "vfmdspanelementsprocessor.h"
#include "vfmdinputlinesequence.h"
#include "textspantreenode.h"
#include "vfmdscopedpointer.h"

void ParagraphHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    ParagraphLineSequence *paragraphLineSequence = new ParagraphLineSequence(lineSequence);
    lineSequence->setChildSequence(paragraphLineSequence);
}

ParagraphLineSequence::ParagraphLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

ParagraphLineSequence::~ParagraphLineSequence()
{
}

void ParagraphLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    UNUSED_ARG(isEndOfParentLineSequence);
    m_lineArray.addLine(currentLine);
}

bool ParagraphLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    return (currentLine.isBlankLine() || !nextLine.isValid());
}

VfmdElementTreeNode* ParagraphLineSequence::endBlock()
{
    m_lineArray.trim();
    VfmdElementTreeNode *paragraphNode = new ParagraphTreeNode();
    VfmdElementTreeNode *spanParseTree = VfmdSpanElementsProcessor::processSpanElements(&m_lineArray, registry());
    bool ok = paragraphNode->setChildNodeIfNotSet(spanParseTree);
    assert(ok);
    return paragraphNode;
}

ParagraphTreeNode::ParagraphTreeNode()
{
}

ParagraphTreeNode::~ParagraphTreeNode()
{
}

void ParagraphTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                   VfmdOutputDevice *outputDevice,
                                   VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        if ((renderOptions & VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) == VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS) {
            renderHtmlIndent(outputDevice, ancestorNodes);
        }
        outputDevice->write("<p>");
        renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        outputDevice->write("</p>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
