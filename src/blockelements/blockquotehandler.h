#ifndef BLOCKQUOTEHANDLER_H
#define BLOCKQUOTEHANDLER_H

#include "vfmdblockelementhandler.h"

class BlockquoteHandler : public VfmdBlockElementHandler {
public:
    BlockquoteHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~BlockquoteHandler() { }
};

class BlockquoteLineSequence : public VfmdBlockLineSequence {
public:
    BlockquoteLineSequence(const VfmdInputLineSequence *parent);
    virtual ~BlockquoteLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine);
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
};

#endif // BLOCKQUOTEHANDLER_H
