#ifndef REFRESOLUTIONBLOCKHANDLER_H
#define REFRESOLUTIONBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class VfmdLinkRefMap;

class RefResolutionBlockHandler : public VfmdBlockElementHandler {
public:
    RefResolutionBlockHandler();
    virtual ~RefResolutionBlockHandler();
    virtual bool isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "ref-resolution-block"; }

    const VfmdLinkRefMap *linkReferenceMap() const;

private:
    VfmdLinkRefMap *m_linkRefMap;
    VfmdByteArray m_rawRefIdString, m_rawUrlString, m_titleContainerString;
    int m_numOfLines;
};

class RefResolutionBlockLineSequence : public VfmdBlockLineSequence {
public:
    RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                   const VfmdByteArray &rawRefIdString,
                                   const VfmdByteArray &rawUrlString,
                                   const VfmdByteArray &titleContainerString,
                                   int numOfLines,
                                   VfmdLinkRefMap *linkRefMap);
    virtual int elementType() const { return VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();

private:
    int m_numOfLinesSeen;
    VfmdByteArray m_rawRefIdString, m_rawUrlString, m_titleContainerString;
    const int m_numOfLinesInSequence;
    VfmdLinkRefMap *m_linkRefMap;
};

#endif // REFRESOLUTIONBLOCKHANDLER_H
