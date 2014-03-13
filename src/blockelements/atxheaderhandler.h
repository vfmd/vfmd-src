#ifndef ATXHEADERHANDLER_H
#define ATXHEADERHANDLER_H

#include "vfmdblockelementhandler.h"

class AtxHeaderHandler : public VfmdBlockElementHandler {
public:
    AtxHeaderHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine) const;
    virtual ~AtxHeaderHandler() { }
    virtual const char *description() const { return "atx-header"; }
};

class AtxHeaderLineSequence : public VfmdBlockLineSequence {
public:
    AtxHeaderLineSequence(const VfmdInputLineSequence *parent);
    virtual ~AtxHeaderLineSequence() { }
    virtual int elementType() const { return VfmdConstants::ATX_HEADER_ELEMENT; }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
private:
    VfmdLine m_headerLine;
};

class AtxHeaderTreeNode : public VfmdElementTreeNode {
public:
    AtxHeaderTreeNode(int headingLevel);
    ~AtxHeaderTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::ATX_HEADER_ELEMENT; }
    virtual const char *elementTypeString() const { return "atx-header"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
private:
    int m_headingLevel;
};

#endif // ATXHEADERHANDLER_H
