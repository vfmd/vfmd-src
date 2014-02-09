#ifndef EMPHASISHANDLER_H
#define EMPHASISHANDLER_H

#include "spanelements/vfmdspanelementhandler.h"
#include <stdio.h>

class EmphasisHandler : public VfmdSpanElementHandler
{
public:
    EmphasisHandler();

    virtual ~EmphasisHandler();

    virtual void identifySpanTagStartingAt(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const;

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
    ~EmphasisTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::EMPHASIS_ELEMENT; }
    virtual const char *elementTypeString() const { return "emphasis"; }

private:
    char m_char;
    int m_repetitionCount;
};

#endif // EMPHASISHANDLER_H
