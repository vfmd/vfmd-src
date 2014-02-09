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
    VfmdPointerArray<const VfmdElementTreeNode> *m_nodes;
};

#endif // VFMDELEMENTTREENODESTACK_H
