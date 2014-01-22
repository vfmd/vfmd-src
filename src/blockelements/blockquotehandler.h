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
    virtual void endBlock();
private:
    VfmdInputLineSequence *m_childSequence;
};

#endif // BLOCKQUOTEHANDLER_H
