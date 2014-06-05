#ifndef EMPHASISHANDLER_H
#define EMPHASISHANDLER_H

#include "vfmdspanelementhandler.h"
#include <stdio.h>

class EmphasisHandler : public VfmdSpanElementHandler
{
public:
    EmphasisHandler();

    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                                   int currentPos,
                                                   VfmdSpanTagStack *stack) const;

    virtual const char *description() const { return "emphasis"; }
};

class OpeningEmphasisTagStackNode : public VfmdOpeningSpanTagStackNode
{
public:
    OpeningEmphasisTagStackNode(char c, int n);

    virtual ~OpeningEmphasisTagStackNode();
    virtual int type() const;
    virtual void populateEquivalentText(VfmdByteArray *ba) const;
    virtual void print() const;

    char character;
    unsigned int repetitionCount;
};

class EmphasisTreeNode : public VfmdElementTreeNode {
public:
    EmphasisTreeNode(char c, int r) : m_char(c), m_repetitionCount(r) { }

    char emphasisCharacter() const { return m_char; }
    int repetitionCount() const { return m_repetitionCount; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::EMPHASIS_ELEMENT; }
    virtual const char *elementTypeString() const { return "emphasis"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
private:
    char m_char;
    int m_repetitionCount;
};

#endif // EMPHASISHANDLER_H
