#ifndef REFRESOLUTIONBLOCKHANDLER_H
#define REFRESOLUTIONBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class RefResolutionBlockHandler : public VfmdBlockElementHandler {
public:
    RefResolutionBlockHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~RefResolutionBlockHandler() { }
    virtual const char *description() const { return "ref-resolution-block"; }
};

class RefResolutionBlockLineSequence : public VfmdBlockLineSequence {
public:
    RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent);
    virtual ~RefResolutionBlockLineSequence() { }
    virtual void processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

private:
    VfmdByteArray m_linkDefText;
    int m_numOfLinesSeen;
    int m_numOfLinesInSequence;
};

#endif // REFRESOLUTIONBLOCKHANDLER_H
