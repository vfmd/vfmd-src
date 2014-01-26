#include "vfmdspanelementhandler.h"

VfmdSpanElementHandler::VfmdSpanElementHandler()
{
}

VfmdSpanElementHandler::~VfmdSpanElementHandler()
{
}

void VfmdSpanElementHandler::identifySpanTagStartingAt(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const
{
    UNUSED_PARAMETER(iterator);
    UNUSED_PARAMETER(stack);
}

bool VfmdSpanElementHandler::identifySpanTagStartingBetween(VfmdLineArrayIterator *fromIterator,
                                                            VfmdLineArrayIterator *toIterator,
                                                            VfmdSpanTagStack *stack) const
{
    UNUSED_PARAMETER(fromIterator);
    UNUSED_PARAMETER(toIterator);
    UNUSED_PARAMETER(stack);
    return false;
}

const char *VfmdSpanElementHandler::description() const
{
    return "generic-span";
}
