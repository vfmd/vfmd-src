#ifndef NULLBLOCKHANDLER_H
#define NULLBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class NullBlockHandler : public VfmdBlockElementHandler
{
public:
    NullBlockHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine,
                                int containingBlockType, bool isAbuttingParagraph);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "null-block"; }
};

class NullBlockLineSequence : public VfmdBlockLineSequence {
public:
    NullBlockLineSequence(const VfmdInputLineSequence *parent);
    virtual int elementType() const { return VfmdConstants::NULL_BLOCK_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();
};

#endif // NULLBLOCKHANDLER_H
