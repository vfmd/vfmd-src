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
    UNUSED_ARG(iterator);
    UNUSED_ARG(stack);
}

bool VfmdSpanElementHandler::identifySpanTagStartingBetween(VfmdLineArrayIterator *fromIterator,
                                                            VfmdLineArrayIterator *toIterator,
                                                            VfmdSpanTagStack *stack) const
{
    UNUSED_ARG(fromIterator);
    UNUSED_ARG(toIterator);
    UNUSED_ARG(stack);
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
