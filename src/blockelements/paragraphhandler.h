#ifndef PARAGRAPHHANDLER_H
#define PARAGRAPHHANDLER_H

#include "vfmdblockelementhandler.h"
#include "vfmdlinearray.h"

class ParagraphHandler : public VfmdBlockElementHandler {
public:
    ParagraphHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~ParagraphHandler() { }
};

class ParagraphLineSequence : public VfmdBlockLineSequence {
public:
    ParagraphLineSequence(const VfmdInputLineSequence *parent);
    virtual ~ParagraphLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual void endBlock();
private:
    VfmdLineArray m_lineArray;
};

#endif // PARAGRAPHHANDLER_H
