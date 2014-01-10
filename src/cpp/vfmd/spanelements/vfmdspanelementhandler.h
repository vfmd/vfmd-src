#ifndef VFMDSPANELEMENTHANDLER_H
#define VFMDSPANELEMENTHANDLER_H

#include "vfmdbytearray.h"

class VfmdLineArrayIterator;
class VfmdSpanTagStack;

#define UNUSED_PARAMETER(x) (void)x;

class VfmdSpanElementHandler
{
public:
    VfmdSpanElementHandler();

    virtual ~VfmdSpanElementHandler();

    /* processSpanTag():
     * If 'iterator' is at the start of a span tag, handle the span tag
     * by pushing to or popping from 'stack', and move 'iterator'
     * to the end of the span tag. */
    virtual void processSpanTag(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const;

    /* A short text describing this syntax (eg. "emphasis", "strikethrough") */
    virtual const char *description() const;
};

#endif // VFMDSPANELEMENTHANDLER_H
