#ifndef VFMDSPANELEMENTHANDLER_H
#define VFMDSPANELEMENTHANDLER_H

#include "vfmdbytearray.h"

class VfmdCharSequence;
class VfmdSpanTagStack;

#define UNUSED_PARAMETER(x) (void)x;

class VfmdSpanElementHandler
{
public:
    VfmdSpanElementHandler();

    virtual ~VfmdSpanElementHandler();

    /* processBytes():
     *
     * Process the input character sequence, modifying the stack if necessary.
     * Returns the number of bytes consumed in the process. */

    virtual unsigned int processBytes(VfmdCharSequence *charSequence, VfmdSpanTagStack *stack) const;

    /* A short text describing this syntax (eg. "emphasis", "strikethrough") */
    virtual const char *description() const;
};

#endif // VFMDSPANELEMENTHANDLER_H
