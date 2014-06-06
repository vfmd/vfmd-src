#ifndef BLOCKQUOTEHANDLER_H
#define BLOCKQUOTEHANDLER_H

#include "vfmdblockelementhandler.h"

class BlockquoteHandler : public VfmdBlockElementHandler {
public:
    BlockquoteHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "blockquote"; }
};

class BlockquoteLineSequence : public VfmdBlockLineSequence {
public:
    BlockquoteLineSequence(const VfmdInputLineSequence *parent);
    virtual ~BlockquoteLineSequence();
    virtual int elementType() const { return VfmdConstants::BLOCKQUOTE_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();
private:
    VfmdInputLineSequence *m_childSequence;
    VfmdLine *m_trailingBlankLine;
};

class BlockquoteTreeNode : public VfmdElementTreeNode {
public:
    BlockquoteTreeNode();

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::BLOCKQUOTE_ELEMENT; }
    virtual const char *elementTypeString() const { return "blockquote"; }
};

#endif // BLOCKQUOTEHANDLER_H
