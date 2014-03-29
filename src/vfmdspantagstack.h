#ifndef VFMDSPANTAGSTACK_H
#define VFMDSPANTAGSTACK_H

#include "vfmdpointerarray.h"
#include "vfmdconstants.h"
#include "vfmdelementtreenode.h"
#include "vfmdspanelementhandler.h"

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
     * making the 'n'th node the top node in the stack. Each popped
     * node is handled as follows:
     *  - If the popped node is not a HTML node, the equivalent
     *    text of the node is added as a contained element of the
     *    'n'th node
     *  - If the popped node is a HTML node, the equivalent tree
     *    node is added as a contained element of the 'n'th node.
     *  - In either case, any contained elements of the popped node
     *    are added as contained elements of the 'n'th node
     * If 'excludeType' is specified, nodes of that type are
     * not popped. */
    void popNodesAboveIndexAsTextFragments(int n,
                                           VfmdConstants::VfmdOpeningSpanTagStackNodeType excludeType = VfmdConstants::UNDEFINED_STACK_NODE);

    /* Remove all nodes with type 'type' in the stack. Each removed
     * node (converted to text) and its contained elements become
     * the contained elements of the node immediately below it. */
    void removeNodesOfTypeAsTextFragments(VfmdConstants::VfmdOpeningSpanTagStackNodeType type) const;

    /* Returns the top node in the stack.
     * Returns null if the stack is empty. */
    VfmdOpeningSpanTagStackNode *topNode() const;

    /* Returns the index of the topmost node in the stack of type 't'. */
    int indexOfTopmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType type) const;

    void collapse();
    VfmdElementTreeNode *baseNodeContents() const;

    void setNonPhrasingHtmlTagSeen(bool yes);
    bool isNonPhrasingHtmlTagSeen() const;

    void setMismatchedHtmlTagSeen(bool yes);
    bool isMismatchedHtmlTagSeen() const;

    void setHtmlCommentSeen(bool yes);
    bool isHtmlCommentSeen() const;

    void print() const;

private:
    /* Prevent copying of this class */
    VfmdSpanTagStack(const VfmdSpanTagStack& other);
    VfmdSpanTagStack& operator=(const VfmdSpanTagStack& other);

    VfmdPointerArray<VfmdOpeningSpanTagStackNode> *m_nodes;
    bool m_isNonPhrasingHtmlTagSeen, m_isMismatchedHtmlTagSeen, m_isHtmlCommentSeen;
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
