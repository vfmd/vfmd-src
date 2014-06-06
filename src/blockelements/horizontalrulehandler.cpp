#include "horizontalrulehandler.h"
#include "vfmdregexp.h"
#include "core/vfmdcommonregexps.h"

bool HorizontalRuleHandler::isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph)
{
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
