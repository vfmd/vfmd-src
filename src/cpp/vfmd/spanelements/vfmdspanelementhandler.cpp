#include "vfmdspanelementhandler.h"

VfmdSpanElementHandler::VfmdSpanElementHandler()
{
}

VfmdSpanElementHandler::~VfmdSpanElementHandler()
{
}

unsigned int VfmdSpanElementHandler::processBytes(VfmdCharSequence *charSequence, VfmdSpanTagStack *stack) const
{
    UNUSED_PARAMETER(charSequence);
    UNUSED_PARAMETER(stack);
    /* Base implementation does not process any bytes */
    return 0;
}

const char *VfmdSpanElementHandler::description() const
{
    return "generic-span";
}
