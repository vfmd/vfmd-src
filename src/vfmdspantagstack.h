#ifndef VFMDSPANTAGSTACK_H
#define VFMDSPANTAGSTACK_H

#include "vfmdpointerarray.h"

struct VfmdSpanTagStackNode
{
public:
    enum Type {
        ASTERISK_EMPHASIS,
        UNDERSCORE_EMPHASIS
    };

    VfmdSpanTagStackNode(Type t, char c /* character data */);
    VfmdSpanTagStackNode(Type t, char c /* character data */, unsigned int n /* repetition count */);

    Type type;
    char character;
    unsigned int repetitionCount;
};

class VfmdSpanTagStack
{
public:
    VfmdSpanTagStack();

    ~VfmdSpanTagStack();

    /* Push a node onto the stack. The stack takes ownership of the object. */
    void push(VfmdSpanTagStackNode *node);

    /* Pops a node off the stack. The stack disowns the object. */
    VfmdSpanTagStackNode *pop();

    /* Pops and frees all objects above 'node' in the stack,
     * making 'node' the topNode.
     * If 'node' is not in the stack, this empties the stack. */
    void popNodesAbove(VfmdSpanTagStackNode *node);

    /* Returns the top node in the stack.
     * Returns null if the stack is empty. */
    VfmdSpanTagStackNode *topNode() const;

    /* Returns the topmost node in the stack of type 't'. */
    VfmdSpanTagStackNode *topmostNodeOfType(VfmdSpanTagStackNode::Type t) const;

    void print() const;

private:
    VfmdPointerArray<VfmdSpanTagStackNode> *m_nodes;
};

#endif // VFMDSPANTAGSTACK_H
