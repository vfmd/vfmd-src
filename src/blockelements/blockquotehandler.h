#ifndef BLOCKQUOTEHANDLER_H
#define BLOCKQUOTEHANDLER_H

#include "vfmdblockelementhandler.h"

class BlockquoteHandler : public VfmdBlockElementHandler {
public:
    void createChildSequence(VfmdInputLineSequence *lineSequence);
};

class BlockquoteLineSequence : public VfmdBlockLineSequence {
public:
    BlockquoteLineSequence(const VfmdInputLineSequence *parent);
    ~BlockquoteLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual void endBlock();
private:
    VfmdInputLineSequence *m_childSequence;
};

#endif // BLOCKQUOTEHANDLER_H
