#ifndef BLOCKQUOTEHANDLER_H
#define BLOCKQUOTEHANDLER_H

#include "vfmdblockelementhandler.h"

class BlockquoteHandler : public VfmdBlockElementHandler {
public:
    BlockquoteHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~BlockquoteHandler() { }
    virtual const char *description() const { return "blockquote"; }
};

class BlockquoteLineSequence : public VfmdBlockLineSequence {
public:
    BlockquoteLineSequence(const VfmdInputLineSequence *parent);
    virtual ~BlockquoteLineSequence();
    virtual int elementType() const { return VfmdConstants::BLOCKQUOTE_ELEMENT; }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
private:
    VfmdInputLineSequence *m_childSequence;
};

class BlockquoteTreeNode : public VfmdElementTreeNode {
public:
    BlockquoteTreeNode();
    ~BlockquoteTreeNode();

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::BLOCKQUOTE_ELEMENT; }
    virtual const char *elementTypeString() const { return "blockquote"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

#endif // BLOCKQUOTEHANDLER_H
