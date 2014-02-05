#ifndef VFMDSPANTAGSTACK_H
#define VFMDSPANTAGSTACK_H

#include "vfmdpointerarray.h"
#include "vfmdconstants.h"
#include "vfmdelementtreenode.h"
#include "spanelements/vfmdspanelementhandler.h"

class VfmdSpanTagStack
{
public:
    VfmdSpanTagStack();

    ~VfmdSpanTagStack();

    unsigned int stackSize() const;
    VfmdOpeningSpanTagStackNode *nodeAt(unsigned int index) const;

    /* Push a node onto the stack. The stack takes ownership of the object. */
    void push(VfmdOpeningSpanTagStackNode *node);

    /* Pops a node off the stack. The stack disowns the object. */
    VfmdOpeningSpanTagStackNode *pop();

    /* Pops all nodes above fenceNode in the stack, thereby
     * making fenceNode the top node in the stack. The popped
     * nodes (converted to text) and their contained elements
     * become fencedNode's contained elements.
     *
     * If fenceNode is not in the stack, this method does nothing
     * and returns false. */
    bool popNodesAboveAsTextFragments(VfmdOpeningSpanTagStackNode *fenceNode);

    /* Returns the top node in the stack.
     * Returns null if the stack is empty. */
    VfmdOpeningSpanTagStackNode *topNode() const;

    /* Returns the topmost node in the stack of type 't'. */
    VfmdOpeningSpanTagStackNode *topmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType type) const;

    void print() const;

private:
    void popNodesAboveIndexAsTextFragments(unsigned int index);

    VfmdPointerArray<VfmdOpeningSpanTagStackNode> *m_nodes;
};

class BaseStackNode : public VfmdOpeningSpanTagStackNode
{
public:
    BaseStackNode() { }
    virtual ~BaseStackNode() { }
    virtual int type() const { return VfmdConstants::BASE_STACK_NODE; }
    virtual void appendEquivalentTextToByteArray(VfmdByteArray *ba) { UNUSED_ARG(ba); }
    virtual void print() const { printf("base of stack"); }
};

#endif // VFMDSPANTAGSTACK_H
