#ifndef PARAGRAPHHANDLER_H
#define PARAGRAPHHANDLER_H

#include "vfmdblockelementhandler.h"
#include "core/vfmdcodespanfilter.h"
#include "core/htmlstatewatcher.h"

class ParagraphHandler : public VfmdBlockElementHandler {
public:
    ParagraphHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const;
    virtual const char *description() const { return "paragraph"; }
};

class ParagraphLineSequence : public VfmdBlockLineSequence {
public:
    ParagraphLineSequence(const VfmdInputLineSequence *parent);
    virtual ~ParagraphLineSequence();
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const;
    virtual void endBlock();
    virtual VfmdPointerArray<const VfmdLine> *linesSinceEndOfBlock();

private:
    int m_containingBlockType;
    VfmdByteArray m_text;
#ifndef VFMD_NO_HTML_AWARE_END_OF_PARAGRAPH
    VfmdCodeSpanFilter m_codeSpanFilter;
    HtmlStateWatcher m_htmlStateWatcher;
    bool m_isAtEndOfParagraph, m_isLookingAhead;
    VfmdPointerArray<const VfmdLine> *m_lookaheadLines;
#endif
};

class ParagraphTreeNode : public VfmdElementTreeNode {
public:
    ParagraphTreeNode();

    void setShouldAvoidWrappingInHtmlPTag(bool avoidPTag);

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual const char *elementTypeString() const { return "paragraph"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    bool m_shouldAvoidWrappingInHtmlPTag;
};

#endif // PARAGRAPHHANDLER_H
