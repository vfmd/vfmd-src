#include "horizontalrulehandler.h"
#include "vfmdregexp.h"
#include "core/vfmdcommonregexps.h"

void HorizontalRuleHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine->isHorizontalRuleLine()) {
        lineSequence->setChildSequence(new HorizontalRuleLineSequence(lineSequence));
    }
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

bool HorizontalRuleLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    // There can be only one line in this line sequence
    return true;
}

VfmdElementTreeNode* HorizontalRuleLineSequence::endBlock()
{
    VfmdElementTreeNode *hrNode = new HorizontalRuleTreeNode;
    return hrNode;
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
