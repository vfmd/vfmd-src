#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "vfmdspantagstack.h"
#include "spanelements/vfmdspanelementhandler.h"

#define ALLOC_CHUNK_SIZE (32)

VfmdSpanTagStack::VfmdSpanTagStack()
{
    m_nodes = new VfmdPointerArray<VfmdOpeningSpanTagStackNode>(ALLOC_CHUNK_SIZE);
}

VfmdSpanTagStack::~VfmdSpanTagStack()
{
    for (unsigned int i = 0; i < m_nodes->size(); i++) {
        delete m_nodes->itemAt(i);
    }
    delete m_nodes;
}

void VfmdSpanTagStack::push(VfmdOpeningSpanTagStackNode *node)
{
    if (node == 0) {
        return;
    }
    m_nodes->append(node);
}

VfmdOpeningSpanTagStackNode *VfmdSpanTagStack::pop()
{
    if (m_nodes->size() == 0) {
        return 0;
    }
    return m_nodes->takeLastItem();
}

void VfmdSpanTagStack::popNodesAbove(VfmdOpeningSpanTagStackNode *node)
{
    while (m_nodes->size() > 0) {
        VfmdOpeningSpanTagStackNode *topNode = m_nodes->lastItem();
        if (node == topNode) {
            break;
        }
        m_nodes->removeLastItem();
        delete topNode;
    }
}

VfmdOpeningSpanTagStackNode *VfmdSpanTagStack::topNode() const
{
    if (m_nodes->size() == 0) {
        return 0;
    }
    return m_nodes->lastItem();
}

VfmdOpeningSpanTagStackNode *VfmdSpanTagStack::topmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType t) const
{
    int i = m_nodes->size() - 1;
    while (i >= 0) {
        VfmdOpeningSpanTagStackNode *node = m_nodes->itemAt(i);
        if (node->type() == t) {
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
        VfmdOpeningSpanTagStackNode *node = m_nodes->itemAt(i);
        printf("  [%d] node: ", i);
        node->print();
        printf("\n");
        i--;
    }
    printf("\n");
}
