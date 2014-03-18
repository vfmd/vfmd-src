#ifndef REFRESOLUTIONBLOCKHANDLER_H
#define REFRESOLUTIONBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"
#include "core/vfmdlinkrefmap.h"

class RefResolutionBlockHandler : public VfmdBlockElementHandler {
public:
    RefResolutionBlockHandler(VfmdLinkRefMap *linkRefMap);
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const;
    virtual ~RefResolutionBlockHandler() { }
    virtual const char *description() const { return "ref-resolution-block"; }

private:
    VfmdLinkRefMap *m_linkRefMap;
};

class RefResolutionBlockLineSequence : public VfmdBlockLineSequence {
public:
    RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                   const VfmdByteArray &firstLineContent,
                                   const VfmdByteArray &secondLineContent,
                                   VfmdLinkRefMap *linkRefMap);
    virtual ~RefResolutionBlockLineSequence() { }
    virtual int elementType() const { return VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

private:
    VfmdLinkRefMap *m_linkRefMap;
    VfmdByteArray m_linkDefText;
    int m_numOfLinesSeen;
    int m_numOfLinesInSequence;
};

#endif // REFRESOLUTIONBLOCKHANDLER_H
