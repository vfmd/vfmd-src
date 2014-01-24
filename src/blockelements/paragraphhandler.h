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
    virtual VfmdElementTreeNode* endBlock();
private:
    VfmdLineArray m_lineArray;
};

class ParagraphTreeNode : public VfmdElementTreeNode {
public:
    ParagraphTreeNode();
    ~ParagraphTreeNode();

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual const char *elementTypeString() const { return "paragraph"; }
};

#endif // PARAGRAPHHANDLER_H
