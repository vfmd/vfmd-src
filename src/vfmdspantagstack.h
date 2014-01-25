#ifndef VFMDSPANTAGSTACK_H
#define VFMDSPANTAGSTACK_H

#include "vfmdpointerarray.h"
#include "vfmdconstants.h"

class VfmdOpeningSpanTagStackNode;

class VfmdSpanTagStack
{
public:
    VfmdSpanTagStack();

    ~VfmdSpanTagStack();

    /* Push a node onto the stack. The stack takes ownership of the object. */
    void push(VfmdOpeningSpanTagStackNode *node);

    /* Pops a node off the stack. The stack disowns the object. */
    VfmdOpeningSpanTagStackNode *pop();

    /* Pops and frees all objects above 'node' in the stack,
     * making 'node' the topNode.
     * If 'node' is not in the stack, this empties the stack. */
    void popNodesAbove(VfmdOpeningSpanTagStackNode *node);

    /* Returns the top node in the stack.
     * Returns null if the stack is empty. */
    VfmdOpeningSpanTagStackNode *topNode() const;

    /* Returns the topmost node in the stack of type 't'. */
    VfmdOpeningSpanTagStackNode *topmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType type) const;

    void print() const;

private:
    VfmdPointerArray<VfmdOpeningSpanTagStackNode> *m_nodes;
};

#endif // VFMDSPANTAGSTACK_H
