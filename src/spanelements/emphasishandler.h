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

#endif // EMPHASISHANDLER_H
