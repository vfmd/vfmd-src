#ifndef BLOCKQUOTE_H
#define BLOCKQUOTE_H

#include "vfmdblockelement.h"

class BlockquoteSyntaxHandler : public VfmdBlockSyntaxHandler {
public:
    VfmdBlockLineSequence *createBlockLineSequence(const VfmdInputLineSequence *lineSequence);
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

#endif // BLOCKQUOTE_H
