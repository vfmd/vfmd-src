#ifndef VFMDSPANELEMENTHANDLER_H
#define VFMDSPANELEMENTHANDLER_H

#include "vfmdbytearray.h"
#include "vfmdconstants.h"

class VfmdLineArrayIterator;
class VfmdSpanTagStack;
class VfmdElementTreeNode;

#define UNUSED_ARG(x) (void)x;

class VfmdSpanElementHandler
{
public:
    VfmdSpanElementHandler();

    virtual ~VfmdSpanElementHandler();

    /* identifySpanTagStartingAt():
     * If this handler identifies a span tag starting just after the 'iterator'
     * position, then this method should handle the tag and move 'iterator' to
     * be just after the end of the identified tag.
     *
     * If no span tag is identified, the 'iterator' should not be moved at all.
     *
     * The stack can be inspected and modified in this method.
     *
     * This method is invoked in the case where the handler is registered with the
     * TRIGGER_AT_TRIGGER_BYTE trigger option (which is the default option). */

    virtual void identifySpanTagStartingAt(VfmdLineArrayIterator *iterator, VfmdSpanTagStack *stack) const;

    /* identifySpanTagStartingBetween():
     * If this handler identifies a span tag that starts between the 'fromIterator'
     * position and the 'toIterator' position, this method should handle the tag
     * and move the iterators such that the 'fromIterator' position is just before
     * the start of the identified tag and the 'toIterator' position is just after
     * the end of the identified tag, and should return 'true'. This method should
     * move 'fromIterator' and 'toIterator' only forwards, if at all, and not move
     * either iterator backwards.
     *
     * If no span tag is identified, between 'fromIterator' and 'toIterator', then
     * this method should return 'false'.
     *
     * The stack can be inspected / pushed to / popped from in this method.
     *
     * This method is invoked in the case where the handler is registered with the
     * TRIGGER_BEFORE_TRIGGER_BYTE trigger option. */

    virtual bool identifySpanTagStartingBetween(VfmdLineArrayIterator *fromIterator, VfmdLineArrayIterator *toIterator,
                                                VfmdSpanTagStack *stack) const;

    /* A short text describing this syntax (eg. "emphasis", "strikethrough") */
    virtual const char *description() const;
};

class VfmdOpeningSpanTagStackNode
{
public:
    VfmdOpeningSpanTagStackNode() : m_containedElements(0) { }

    virtual ~VfmdOpeningSpanTagStackNode() { }

    virtual int type() const { return VfmdConstants::UNDEFINED_STACK_NODE; }

    virtual void populateEquivalentText(VfmdByteArray *ba) const {
        UNUSED_ARG(ba);
    }

    virtual void print() const { }

    void appendToContainedElements(VfmdElementTreeNode *elementsToAppend);
    void appendToContainedElements(const VfmdByteArray &textToAppend);
    void appendToContainedElements(VfmdOpeningSpanTagStackNode *node);

private:
    VfmdElementTreeNode *m_containedElements;
};

#endif // VFMDSPANELEMENTHANDLER_H
