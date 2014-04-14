#include "horizontalrulehandler.h"
#include "vfmdregexp.h"
#include "core/vfmdcommonregexps.h"

bool HorizontalRuleHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine,
                                           int containingBlockType, bool isAbuttingParagraph)
{
    UNUSED_ARG(nextLine);
    UNUSED_ARG(containingBlockType);
    UNUSED_ARG(isAbuttingParagraph);
    return (currentLine->isHorizontalRuleLine());
}

void HorizontalRuleHandler::createLineSequence(VfmdInputLineSequence *parentLineSequence) const
{
    HorizontalRuleLineSequence *s = new HorizontalRuleLineSequence(parentLineSequence);
    parentLineSequence->setChildSequence(s);
}

HorizontalRuleLineSequence::HorizontalRuleLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent)
{
}

void HorizontalRuleLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
}

bool HorizontalRuleLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    // There can be only one line in this line sequence
    return true;
}

void HorizontalRuleLineSequence::endBlock()
{
    setBlockParseTree(new HorizontalRuleTreeNode);
}

void HorizontalRuleTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        bool shouldUseSelfClosingTags = ((renderOptions & VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS) ==  VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS);
        outputDevice->write(shouldUseSelfClosingTags? "<hr />\n" : "<hr>\n");
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
