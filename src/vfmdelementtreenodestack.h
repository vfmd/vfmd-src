#ifndef VFMDELEMENTTREENODESTACK_H
#define VFMDELEMENTTREENODESTACK_H

#include "vfmdelementtreenode.h"
#include "vfmdpointerarray.h"

class VfmdElementTreeNodeStack {
public:
    VfmdElementTreeNodeStack();
    ~VfmdElementTreeNodeStack();

    unsigned int size() const;
    const VfmdElementTreeNode* nodeAt(unsigned int index) const;

    void push(const VfmdElementTreeNode *node);
    const VfmdElementTreeNode* pop();

private:
    /* Prevent copying of this class */
    VfmdElementTreeNodeStack(const VfmdElementTreeNodeStack& other);
    VfmdElementTreeNodeStack& operator=(const VfmdElementTreeNodeStack& other);

    VfmdPointerArray<const VfmdElementTreeNode> *m_nodes;
};

#endif // VFMDELEMENTTREENODESTACK_H
