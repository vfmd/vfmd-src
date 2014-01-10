#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include "vfmdblockelement.h"
#include "vfmdlinearray.h"

class ParagraphSyntaxHandler : public VfmdBlockSyntaxHandler {
public:
    VfmdBlockLineSequence *createBlockLineSequence(const VfmdInputLineSequence *lineSequence);
};

class ParagraphLineSequence : public VfmdBlockLineSequence {
public:
    ParagraphLineSequence(const VfmdInputLineSequence *parent);
    void processLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    bool isAtEnd() const;
private:
    bool m_isAtEnd;
    VfmdLineArray m_lineArray;
};

#endif // PARAGRAPH_H
