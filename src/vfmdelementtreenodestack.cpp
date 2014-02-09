#include "vfmdelementtreenodestack.h"

VfmdElementTreeNodeStack::VfmdElementTreeNodeStack()
    : m_nodes(new VfmdPointerArray<const VfmdElementTreeNode>(8))
{
}

VfmdElementTreeNodeStack::~VfmdElementTreeNodeStack()
{
    delete m_nodes;
}

unsigned int VfmdElementTreeNodeStack::size() const
{
    return m_nodes->size();
}

const VfmdElementTreeNode* VfmdElementTreeNodeStack::nodeAt(unsigned int index) const
{
    return m_nodes->itemAt(index);
}

void VfmdElementTreeNodeStack::push(const VfmdElementTreeNode *node)
{
    m_nodes->append(node);
}

const VfmdElementTreeNode* VfmdElementTreeNodeStack::pop()
{
    return m_nodes->takeLastItem();
}
