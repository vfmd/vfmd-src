#ifndef PARAGRAPHHANDLER_H
#define PARAGRAPHHANDLER_H

#include "vfmdblockelementhandler.h"
#include "vfmdlinearray.h"

class ParagraphHandler : public VfmdBlockElementHandler {
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

#endif // PARAGRAPHHANDLER_H
