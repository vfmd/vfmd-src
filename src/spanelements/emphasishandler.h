#ifndef EMPHASISHANDLER_H
#define EMPHASISHANDLER_H

#include "spanelements/vfmdspanelementhandler.h"

class EmphasisHandler : public VfmdSpanElementHandler
{
public:
    EmphasisHandler();

    virtual ~EmphasisHandler();

    virtual void processSpanTag(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const;

    virtual const char *description() const { return "emphasis"; }
};

class OpeningEmphasisTagStackNode : public VfmdOpeningSpanTagStackNode
{
public:
    OpeningEmphasisTagStackNode(char c, int n);

    virtual ~OpeningEmphasisTagStackNode();
    virtual int type() const;
    virtual void appendEquivalentTextToByteArray(VfmdByteArray *ba);
    virtual void print() const;

    char character;
    unsigned int repetitionCount;
};

#endif // EMPHASISHANDLER_H
