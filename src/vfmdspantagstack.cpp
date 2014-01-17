#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "vfmdspantagstack.h"

#define ALLOC_CHUNK_SIZE (32)

VfmdSpanTagStackNode::VfmdSpanTagStackNode(VfmdSpanTagStackNode::Type t, char c)
    : type(t), character(c), repetitionCount(1)
{
}

VfmdSpanTagStackNode::VfmdSpanTagStackNode(VfmdSpanTagStackNode::Type t, char c, unsigned int n)
    : type(t), character(c), repetitionCount(n)
{
}

VfmdSpanTagStack::VfmdSpanTagStack()
{
    m_nodes = new VfmdPointerArray<VfmdSpanTagStackNode>(ALLOC_CHUNK_SIZE);
}

VfmdSpanTagStack::~VfmdSpanTagStack()
{
    for (unsigned int i = 0; i < m_nodes->size(); i++) {
        delete m_nodes->itemAt(i);
    }
    delete m_nodes;
}

void VfmdSpanTagStack::push(VfmdSpanTagStackNode *node)
{
    if (node == 0) {
        return;
    }
    m_nodes->append(node);
}

VfmdSpanTagStackNode *VfmdSpanTagStack::pop()
{
    if (m_nodes->size() == 0) {
        return 0;
    }
    return m_nodes->takeLastItem();
}

void VfmdSpanTagStack::popNodesAbove(VfmdSpanTagStackNode *node)
{
    while (m_nodes->size() > 0) {
        VfmdSpanTagStackNode *topNode = m_nodes->lastItem();
        if (node == topNode) {
            break;
        }
        m_nodes->removeLastItem();
        delete topNode;
    }
}

VfmdSpanTagStackNode *VfmdSpanTagStack::topNode() const
{
    if (m_nodes->size() == 0) {
        return 0;
    }
    return m_nodes->lastItem();
}

VfmdSpanTagStackNode *VfmdSpanTagStack::topmostNodeOfType(VfmdSpanTagStackNode::Type t) const
{
    int i = m_nodes->size() - 1;
    while (i >= 0) {
        VfmdSpanTagStackNode *node = m_nodes->itemAt(i);
        if (node->type == t) {
            return node;
        }
        i--;
    }
    return 0;
}

void VfmdSpanTagStack::print() const
{
    printf("STACK: (%d nodes)\n", m_nodes->size());
    int i = m_nodes->size() - 1;
    while (i >= 0) {
        VfmdSpanTagStackNode *node = m_nodes->itemAt(i);
        printf("  [%d] node: { %d, '%c' x %d }\n", i, node->type, node->character, node->repetitionCount);
        i--;
    }
    printf("\n");
}
