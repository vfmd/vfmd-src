#ifndef REFRESOLUTIONBLOCKHANDLER_H
#define REFRESOLUTIONBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"
#include "core/vfmdlinkrefmap.h"

class RefResolutionBlockHandler : public VfmdBlockElementHandler {
public:
    RefResolutionBlockHandler(VfmdLinkRefMap *linkRefMap);
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const;
    virtual const char *description() const { return "ref-resolution-block"; }

private:
    VfmdLinkRefMap *m_linkRefMap;
};

class RefResolutionBlockLineSequence : public VfmdBlockLineSequence {
public:
    RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                   int numOfLines,
                                   const VfmdByteArray &linkDefText,
                                   VfmdLinkRefMap *linkRefMap);
    virtual int elementType() const { return VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

private:
    int m_numOfLinesSeen;
    const int m_numOfLinesInSequence;
    VfmdByteArray m_linkDefText;
    VfmdLinkRefMap *m_linkRefMap;
};

#endif // REFRESOLUTIONBLOCKHANDLER_H
