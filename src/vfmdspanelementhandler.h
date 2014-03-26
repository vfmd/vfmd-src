#ifndef VFMDSPANELEMENTHANDLER_H
#define VFMDSPANELEMENTHANDLER_H

#include "vfmdbytearray.h"
#include "vfmdconstants.h"
#include "vfmdelementtreenode.h"

class VfmdSpanTagStack;
class VfmdElementTreeNode;

#define UNUSED_ARG(x) (void)x;

class VfmdSpanElementHandler
{
public:
    VfmdSpanElementHandler();

    virtual ~VfmdSpanElementHandler();

    /* identifySpanTagStartingAt():
     * If this handler identifies a span tag starting just after 'currentPos',
     * then this method should handle the tag and return the number of bytes consumed.
     * The stack can be inspected and modified.
     *
     * If no span tag is identified, this method should not modify the stack and
     * return 0.
     *
     * This method is invoked in the case where the handler is registered with the
     * TRIGGER_AT_TRIGGER_BYTE trigger option (which is the default option). */

    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                                   int currentPos,
                                                   VfmdSpanTagStack *stack) const;

    /* identifySpanTagStartingBetween():
     * If this handler identifies a span tag that starts between 'fromPos' and 'toPos',
     * this method should identify the part before
     * the start of the span tag as a text fragment, handle the span tag, and
     * return the number of bytes consumed.
     * The stack can be inspected and modified.
     *
     * If no span tag is identified, this method should not modify the stack and
     * return 0.
     *
     * This method is invoked in the case where the handler is registered with the
     * TRIGGER_BEFORE_TRIGGER_BYTE trigger option. */

    virtual int identifySpanTagStartingBetween(const VfmdByteArray &text,
                                                        int fromPos, int toPos,
                                                        VfmdSpanTagStack *stack) const;

    /* A short text describing this syntax (eg. "emphasis", "strikethrough") */
    virtual const char *description() const;

private:
    /* Prevent copying of this class */
    VfmdSpanElementHandler(const VfmdSpanElementHandler& other);
    VfmdSpanElementHandler& operator=(const VfmdSpanElementHandler& other);
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

private:
    void appendToContainedElements(VfmdOpeningSpanTagStackNode *node);

    /* Prevent copying of this class */
    VfmdOpeningSpanTagStackNode(const VfmdOpeningSpanTagStackNode& other);
    VfmdOpeningSpanTagStackNode& operator=(const VfmdOpeningSpanTagStackNode& other);

    VfmdElementTreeNode *m_containedElements;

    friend class VfmdElementTreeNode;
    friend class VfmdSpanTagStack;
};

#endif // VFMDSPANELEMENTHANDLER_H
