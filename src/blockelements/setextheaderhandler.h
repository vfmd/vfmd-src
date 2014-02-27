#ifndef SETEXTHEADERHANDLER_H
#define SETEXTHEADERHANDLER_H

#include "vfmdblockelementhandler.h"
#include "vfmdline.h"

class SetextHeaderHandler : public VfmdBlockElementHandler {
public:
    SetextHeaderHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~SetextHeaderHandler() { }
    virtual const char *description() const { return "setext-header"; }
};

class SetextHeaderLineSequence : public VfmdBlockLineSequence {
public:
    SetextHeaderLineSequence(const VfmdInputLineSequence *parent);
    virtual ~SetextHeaderLineSequence() { }
    virtual int elementType() const { return VfmdConstants::SETEXT_HEADER_ELEMENT; }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
private:
    int m_numOfLinesSeen;
    int m_headingLevel;
    VfmdLine m_firstLine;
};

class SetextHeaderTreeNode : public VfmdElementTreeNode {
public:
    SetextHeaderTreeNode(int headingLevel);
    ~SetextHeaderTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::SETEXT_HEADER_ELEMENT; }
    virtual const char *elementTypeString() const { return "setext-header"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
private:
    int m_headingLevel;
};

#endif // SETEXTHEADERHANDLER_H
