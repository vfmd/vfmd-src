#include "vfmdspanelementhandler.h"

VfmdSpanElementHandler::VfmdSpanElementHandler()
{
}

VfmdSpanElementHandler::~VfmdSpanElementHandler()
{
}

void VfmdSpanElementHandler::processSpanTag(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const
{
    UNUSED_PARAMETER(iterator);
    UNUSED_PARAMETER(stack);
}

const char *VfmdSpanElementHandler::description() const
{
    return "generic-span";
}
