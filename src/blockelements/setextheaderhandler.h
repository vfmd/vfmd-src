#ifndef SETEXTHEADERHANDLER_H
#define SETEXTHEADERHANDLER_H

#include "vfmdblockelementhandler.h"
#include "vfmdline.h"

class SetextHeaderHandler : public VfmdBlockElementHandler {
public:
    SetextHeaderHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "setext-header"; }
};

class SetextHeaderLineSequence : public VfmdBlockLineSequence {
public:
    SetextHeaderLineSequence(const VfmdInputLineSequence *parent);
    virtual int elementType() const { return VfmdConstants::SETEXT_HEADER_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();
private:
    int m_numOfLinesSeen;
    int m_headingLevel;
    VfmdByteArray m_firstLineContent;
};

class SetextHeaderTreeNode : public VfmdElementTreeNode {
public:
    SetextHeaderTreeNode(int headingLevel);

    int headingLevel() const { return m_headingLevel; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::SETEXT_HEADER_ELEMENT; }
    virtual const char *elementTypeString() const { return "setext-header"; }

private:
    int m_headingLevel;
};

#endif // SETEXTHEADERHANDLER_H
