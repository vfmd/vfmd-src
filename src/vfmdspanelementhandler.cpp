#include "vfmdspanelementhandler.h"
#include "vfmdelementtreenode.h"
#include "textspantreenode.h"
#include "spanelements/htmltaghandler.h"

VfmdSpanElementHandler::VfmdSpanElementHandler()
{
}

VfmdSpanElementHandler::~VfmdSpanElementHandler()
{
}

int VfmdSpanElementHandler::identifySpanTagStartingAt(const VfmdByteArray &text,
                                                               int currentPos,
                                                               VfmdSpanTagStack *stack) const
{
    UNUSED_ARG(text);
    UNUSED_ARG(currentPos);
    UNUSED_ARG(stack);
    return 0;
}

int VfmdSpanElementHandler::identifySpanTagStartingBetween(const VfmdByteArray &text,
                                                                    int fromPos, int toPos,
                                                                    VfmdSpanTagStack *stack) const
{
    UNUSED_ARG(text);
    UNUSED_ARG(fromPos);
    UNUSED_ARG(toPos);
    UNUSED_ARG(stack);
    return 0;
}

const char *VfmdSpanElementHandler::description() const
{
    return "generic-span";
}

void VfmdOpeningSpanTagStackNode::appendToContainedElements(VfmdElementTreeNode *elementsToAppend)
{
    if (elementsToAppend == 0) {
        return;
    }
    if (m_containedElements) {
        m_containedElements->appendSubtreeToEndOfSequence(elementsToAppend);
    } else {
        m_containedElements = elementsToAppend;
    }
}

void VfmdOpeningSpanTagStackNode::appendToContainedElements(const VfmdByteArray &textToAppend)
{
    if (m_containedElements) {
        m_containedElements->appendTextToEndOfSequence(textToAppend);
    } else {
        m_containedElements = new TextSpanTreeNode(textToAppend);
    }
}

void VfmdOpeningSpanTagStackNode::appendToContainedElements(VfmdOpeningSpanTagStackNode *otherNode)
{
    if (otherNode->type() == VfmdConstants::RAW_HTML_STACK_NODE) {
        // Unclosed HTML tags should be interpreted as HTML
        OpeningHtmlTagStackNode *rawHtmlStackNode = dynamic_cast<OpeningHtmlTagStackNode *>(otherNode);
        HtmlTreeNode *htmlTreeNode = rawHtmlStackNode->toUnclosedStartHtmlTagTreeNode();
        appendToContainedElements(htmlTreeNode);
    } else {
        // Unclosed non-HTML tags should be interpreted as text
        VfmdByteArray equivalentText;
        otherNode->populateEquivalentText(&equivalentText);
        appendToContainedElements(equivalentText);
    }
    appendToContainedElements(otherNode->m_containedElements);
    otherNode->m_containedElements = 0;
}
