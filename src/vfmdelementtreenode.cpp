#include <assert.h>
#include "vfmdelementtreenode.h"

VfmdElementTreeNode::VfmdElementTreeNode()
    : m_nextSibling(0)
    , m_lastSibling(0)
    , m_firstChild(0)
{
}

VfmdElementTreeNode::~VfmdElementTreeNode()
{
}

void VfmdElementTreeNode::addAsLastSiblingNode(VfmdElementTreeNode *node)
{
    if (m_nextSibling == 0) {
        // This is the end node
        insertAsNextNode(node);
    } else {
        // This is not the end node
        lastSiblingNode()->insertAsNextNode(node);
    }
    m_lastSibling = node;
}

void VfmdElementTreeNode::addAsLastChildNode(VfmdElementTreeNode *node)
{
    if (m_firstChild == 0) {
        // No children yet
        insertAsFirstChild(node);
    } else {
        // There are children
        m_firstChild->addAsLastSiblingNode(node);
    }
}

bool VfmdElementTreeNode::hasNext() const
{
    return (m_nextSibling != 0);
}

bool VfmdElementTreeNode::hasChildren() const
{
    return (m_firstChild != 0);
}

VfmdElementTreeNode* VfmdElementTreeNode::nextNode() const
{
    return m_nextSibling;
}

VfmdElementTreeNode* VfmdElementTreeNode::firstChildNode() const
{
    return m_firstChild;
}

void VfmdElementTreeNode::insertAsNextNode(VfmdElementTreeNode *node)
{
    assert(node->m_nextSibling == 0);
    node->m_nextSibling = m_nextSibling;
    m_nextSibling = node;
}

void VfmdElementTreeNode::insertAsFirstChild(VfmdElementTreeNode *node)
{
    assert(node->m_nextSibling == 0);
    node->m_nextSibling = m_firstChild;
    m_firstChild = node;
}

VfmdElementTreeNode *VfmdElementTreeNode::lastSiblingNode() const
{
    if (m_nextSibling == 0) {
        return 0;
    }

    if (m_lastSibling != 0) {
        return m_lastSibling;
    }

    // Need to find the last sibling node
    VfmdElementTreeNode *nextSibling = m_nextSibling;
    VfmdElementTreeNode *lastSibling = 0;
    while (nextSibling) {
        if (nextSibling->m_lastSibling != 0) {
            lastSibling = nextSibling->m_lastSibling;
            break;
        }
        lastSibling = nextSibling;
        nextSibling = nextSibling->m_nextSibling;
    }
    return lastSibling;
}
