#ifndef REFRESOLUTIONBLOCKHANDLER_H
#define REFRESOLUTIONBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class VfmdLinkRefMap;

class RefResolutionBlockHandler : public VfmdBlockElementHandler {
public:
    RefResolutionBlockHandler();
    virtual ~RefResolutionBlockHandler();
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const;
    virtual const char *description() const { return "ref-resolution-block"; }

    const VfmdLinkRefMap *linkReferenceMap() const;

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
    virtual void endBlock();

private:
    int m_numOfLinesSeen;
    const int m_numOfLinesInSequence;
    VfmdByteArray m_linkDefText;
    VfmdLinkRefMap *m_linkRefMap;
};

#endif // REFRESOLUTIONBLOCKHANDLER_H
