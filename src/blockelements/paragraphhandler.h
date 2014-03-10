#ifndef PARAGRAPHHANDLER_H
#define PARAGRAPHHANDLER_H

#include "vfmdblockelementhandler.h"
#include "core/vfmdcodespanfilter.h"
#include "core/htmlstatewatcher.h"

class ParagraphHandler : public VfmdBlockElementHandler {
public:
    ParagraphHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~ParagraphHandler() { }
    virtual const char *description() const { return "paragraph"; }
};

class ParagraphLineSequence : public VfmdBlockLineSequence {
public:
    ParagraphLineSequence(const VfmdInputLineSequence *parent);
    virtual ~ParagraphLineSequence();
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
    virtual VfmdPointerArray<const VfmdLine> *linesSinceEndOfBlock() const;

private:
    int m_containingBlockType;
    VfmdPointerArray<const VfmdLine> * m_lines;
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    VfmdCodeSpanFilter m_codeSpanFilter;
    HtmlStateWatcher m_htmlStateWatcher;
#endif
    bool m_isAtEndOfParagraph, m_isLookingAhead;
    VfmdPointerArray<const VfmdLine> *m_lookaheadLines;
};

class ParagraphTreeNode : public VfmdElementTreeNode {
public:
    ParagraphTreeNode();
    ~ParagraphTreeNode();

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual const char *elementTypeString() const { return "paragraph"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

#endif // PARAGRAPHHANDLER_H
