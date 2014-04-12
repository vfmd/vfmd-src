#ifndef HORIZONTALRULEHANDLER_H
#define HORIZONTALRULEHANDLER_H

#include "vfmdblockelementhandler.h"

class HorizontalRuleHandler : public VfmdBlockElementHandler {
public:
    HorizontalRuleHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const;
    virtual const char *description() const { return "horizontal-rule"; }
};

class HorizontalRuleLineSequence : public VfmdBlockLineSequence {
public:
    HorizontalRuleLineSequence(const VfmdInputLineSequence *parent);
    virtual int elementType() const { return VfmdConstants::HORIZONTAL_RULE_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const;
    virtual void endBlock();
};

class HorizontalRuleTreeNode : public VfmdElementTreeNode {
public:
    HorizontalRuleTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::HORIZONTAL_RULE_ELEMENT; }
    virtual const char *elementTypeString() const { return "horizontal-rule"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

#endif // HORIZONTALRULEHANDLER_H
