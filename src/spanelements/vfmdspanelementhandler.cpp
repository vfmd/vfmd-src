#include "vfmdspanelementhandler.h"
#include "vfmdelementtreenode.h"
#include "textspantreenode.h"

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

void VfmdOpeningSpanTagStackNode::appendToContainedElements(VfmdElementTreeNode *elementsToAppend)
{
    if (m_containedElements) {
        m_containedElements->appendSiblings(elementsToAppend);
    } else {
        m_containedElements = elementsToAppend;
    }
}

void VfmdOpeningSpanTagStackNode::appendToContainedElements(const VfmdByteArray &textToAppend)
{
    if (m_containedElements) {
        m_containedElements->appendText(textToAppend);
    } else {
        m_containedElements = new TextSpanTreeNode(textToAppend);
    }
}
