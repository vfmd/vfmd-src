#ifndef PARAGRAPHHANDLER_H
#define PARAGRAPHHANDLER_H

#include "vfmdblockelementhandler.h"
#include "core/vfmdcodespanfilter.h"
#include "core/htmlstatewatcher.h"

class ParagraphLineSequence : public VfmdBlockLineSequence {
public:
    ParagraphLineSequence(const VfmdInputLineSequence *parent);
    virtual ~ParagraphLineSequence();
    virtual int elementType() const { return VfmdConstants::PARAGRAPH_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();

    // linesSinceEndOfParagraph():
    // If there are lines passed as 'currentLine' to this
    // block's processBlockLine() that are not consumed in the block,
    // those lines are returned by this method, in the same order in
    // which they were given to this block.
    VfmdPointerArray<const VfmdLine> *linesSinceEndOfParagraph();

    // nextBlockHandler():
    // If the paragraph sequence knows what the following block is after
    // this block has ended with an 'endBlock()', this
    // method returns the block handler for the following block. Else,
    // this method returns 0.
    // If this method returns a block handler, then that handler's
    // VfmdBlockElementHandler::isStartOfBlock() is guaranteed to
    // have returned true.
    VfmdBlockElementHandler *nextBlockHandler() const;

private:
    bool canEndBeforeLine(const VfmdLine *line, bool isInVerbatimHtmlMode);

    const int m_containingBlockType;
    VfmdByteArray m_text;
    const VfmdPointerArray<VfmdBlockElementHandler> *m_blockHandlersThatCanAbutParagraph;
    VfmdBlockElementHandler *m_nextBlockHandler;
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
