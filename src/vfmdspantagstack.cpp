#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "vfmdspantagstack.h"
#include "vfmdspanelementhandler.h"
#include "vfmdoutputdevice.h"

#define ALLOC_CHUNK_SIZE (32)

VfmdSpanTagStack::VfmdSpanTagStack()
{
    m_nodes = new VfmdPointerArray<VfmdOpeningSpanTagStackNode>(ALLOC_CHUNK_SIZE);
    BaseStackNode *baseNode = new BaseStackNode;
    push(baseNode);
}

VfmdSpanTagStack::~VfmdSpanTagStack()
{
    for (unsigned int i = 0; i < m_nodes->size(); i++) {
        delete m_nodes->itemAt(i);
    }
    delete m_nodes;
}

unsigned int VfmdSpanTagStack::stackSize() const
{
    return m_nodes->size();
}

VfmdOpeningSpanTagStackNode *VfmdSpanTagStack::nodeAt(unsigned int index) const
{
    return m_nodes->itemAt(index);
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
    if (m_nodes->size() <= 1) {
        // Cannot pop the base node
        return 0;
    }
    return m_nodes->takeLastItem();
}

void VfmdSpanTagStack::popNodesAboveIndexAsTextFragments(int index)
{
    unsigned int count = stackSize();
    if ((index < 0) || (index >= (int) (count - 1))) {
        return;
    }

    VfmdOpeningSpanTagStackNode *fenceNode = nodeAt(index);

    // Convert to-be-popped nodes to text fragments
    for (int i = index + 1;
         i < (int) count;
         i++) {
        VfmdOpeningSpanTagStackNode *node = nodeAt(i);
        fenceNode->appendToContainedElements(node);
    }

    // Pop nodes and free them
    for (int i = count - 1;
         i > (int) index;
         i--) {
        VfmdOpeningSpanTagStackNode *node = m_nodes->takeItemAt(i);
        delete node;
    }

    assert(index == (int) (stackSize() - 1));
}

void VfmdSpanTagStack::removeNodesOfTypeAsTextFragments(VfmdConstants::VfmdOpeningSpanTagStackNodeType t) const
{
    assert(t != VfmdConstants::BASE_STACK_NODE);
    int i = m_nodes->size() - 1;
    while (i >= 1) {
        VfmdOpeningSpanTagStackNode *node = m_nodes->itemAt(i);
        if (node->type() == t) {
            m_nodes->itemAt(i - 1)->appendToContainedElements(node);
            m_nodes->removeItemAt(i);
            delete node;
        }
        i--;
    }
}

VfmdOpeningSpanTagStackNode *VfmdSpanTagStack::topNode() const
{
    assert(m_nodes->size() > 0);
    if (m_nodes->size() == 0) {
        return 0;
    }
    return m_nodes->lastItem();
}

int VfmdSpanTagStack::indexOfTopmostNodeOfType(VfmdConstants::VfmdOpeningSpanTagStackNodeType t) const
{
    int i = m_nodes->size() - 1;
    while (i >= 0) {
        VfmdOpeningSpanTagStackNode *node = m_nodes->itemAt(i);
        if (node->type() == t) {
            return i;
        }
        i--;
    }
    return -1;
}

VfmdElementTreeNode *VfmdSpanTagStack::collapse()
{
    assert(stackSize() >= 1);
    assert(nodeAt(0)->type() == VfmdConstants::BASE_STACK_NODE);
    popNodesAboveIndexAsTextFragments(0);
    BaseStackNode *baseNode = dynamic_cast<BaseStackNode *>(nodeAt(0));
    assert(baseNode);
    if (baseNode) {
        return baseNode->m_containedElements;
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
        VfmdElementTreeNode *tree = node->m_containedElements;
        if (tree) {
            printf("Contained elements for node [%d]:\n", i);
            VfmdConsoleOutputDevice console;
            tree->renderSequence(VfmdConstants::TREE_FORMAT,
                                 VfmdConstants::TREE_RENDER_INCLUDES_TEXT,
                                 &console);
            printf("\n");
        }
        i--;
    }
    printf("\n");
}
