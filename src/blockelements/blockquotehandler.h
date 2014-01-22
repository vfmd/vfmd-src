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
    void processLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    bool isAtEnd() const;
private:
    bool m_isAtEnd;
    VfmdInputLineSequence *m_childSequence;
};

#endif // BLOCKQUOTEHANDLER_H
