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

    /* Pops all nodes with index above 'n' in the stack, thereby
     * making the 'n'th node the top node in the stack. The popped
     * nodes (converted to text) and their contained elements
     * become the contained elements of the 'n'th node. */
    void popNodesAboveIndexAsTextFragments(int n);

    /* Returns the top node in the stack.
     * Returns null if the stack is empty. */
    VfmdOpeningSpanTagStackNode *topNode() const;

    /* Returns the index of the topmost node in the stack of type 't'. */
    int indexOfTopmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType type) const;

    VfmdElementTreeNode *collapse();

    void print() const;

private:
    /* Prevent copying of this class */
    VfmdSpanTagStack(const VfmdSpanTagStack& other);
    VfmdSpanTagStack& operator=(const VfmdSpanTagStack& other);

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
