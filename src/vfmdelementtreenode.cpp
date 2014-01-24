#include <assert.h>
#include <stdio.h>
#include "vfmdelementtreenode.h"
#include "textspantreenode.h"

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

void VfmdElementTreeNode::setChildSubtree(VfmdElementTreeNode *node)
{
    assert(m_firstChild == 0);
    m_firstChild = node;
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

void VfmdElementTreeNode::appendEquivalentTextToTextSpanNode(TextSpanTreeNode *node)
{
    UNUSED_ARG(node);
    /* Base implementation does nothing */
}

void VfmdElementTreeNode::printSubtreeSequence(const VfmdByteArray &padding) const
{
    for (const VfmdElementTreeNode *node = this;
         node != 0;
         node = node->nextNode()) {
        node->printSubtree(padding);
    }
    if (!hasChildren()) {
        padding.print(); printf("\n");
    }
}

void VfmdElementTreeNode::printSubtree(const VfmdByteArray &padding) const
{
    const char *classification = "";
    switch (elementClassification()) {
    case UNDEFINED: classification = "UNDEFINED"; break;
    case BLOCK:     classification = "BLOCK"; break;
    case SPAN:      classification = "SPAN"; break;
    case TEXTSPAN:  classification = "TEXTSPAN"; break;
    }
    padding.print();
    printf("+- %s/%s\n", classification, elementTypeString());
    VfmdByteArray subsequentPadding = padding;
    if (hasNext()) {
        subsequentPadding.append("|  ");
    } else {
        subsequentPadding.append("   ");
    }
    if (elementClassification() == TEXTSPAN && elementType() == VfmdConstants::TEXTSPAN_ELEMENT) {
        const TextSpanTreeNode *textSpanNode = dynamic_cast<const TextSpanTreeNode *>(this);
        if (textSpanNode) {
            textSpanNode->text().print(subsequentPadding, true);
            printf("\n");
        }
    }
    if (hasChildren()) {
        subsequentPadding.print(); printf("|\n");
        firstChildNode()->printSubtreeSequence(subsequentPadding);
    }
}
