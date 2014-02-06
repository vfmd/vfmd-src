#include <assert.h>
#include <stdio.h>
#include "vfmdelementtreenode.h"
#include "textspantreenode.h"
#include "spanelements/vfmdspanelementhandler.h"

VfmdElementTreeNode::VfmdElementTreeNode()
    : m_nextSibling(0)
    , m_lastSibling(0)
    , m_firstChild(0)
{
}

VfmdElementTreeNode::~VfmdElementTreeNode()
{
}

void VfmdElementTreeNode::appendSiblings(VfmdElementTreeNode *subtree)
{
    VfmdElementTreeNode *lastNode = lastSiblingNode();
    if ((subtree->elementClassification() == VfmdElementTreeNode::TEXTSPAN) &&
        (lastNode->elementClassification() == VfmdElementTreeNode::TEXTSPAN)) {
        // Do not allow two text nodes side-by-side.
        // Merge them into a single text node.
        TextSpanTreeNode *textNode = dynamic_cast<TextSpanTreeNode *>(subtree);
        TextSpanTreeNode *textLastNode = dynamic_cast<TextSpanTreeNode *>(lastNode);
        assert(textNode != 0);
        assert(textLastNode != 0);
        textLastNode->appendText(textNode->text());
        bool ok = textLastNode->setNextNodeIfNotSet(subtree->nextNode());
        assert(ok);
        m_lastSibling = subtree->lastSiblingNode();
        delete subtree;
    } else {
        bool ok = lastNode->setNextNodeIfNotSet(subtree);
        assert(ok);
        m_lastSibling = subtree->lastSiblingNode();
    }
}

void VfmdElementTreeNode::appendChildren(VfmdElementTreeNode *subtree)
{
    if (m_firstChild == 0) {
        m_firstChild = subtree;
    } else {
        m_firstChild->appendSiblings(subtree);
    }
}

void VfmdElementTreeNode::appendText(const VfmdByteArray &ba)
{
    VfmdElementTreeNode *lastNode = lastSiblingNode();
    if (lastNode->elementClassification() == VfmdElementTreeNode::TEXTSPAN) {
        TextSpanTreeNode *textLastNode = dynamic_cast<TextSpanTreeNode *>(lastNode);
        assert(textLastNode != 0);
        textLastNode->appendText(ba);
    } else {
        TextSpanTreeNode *textNode = new TextSpanTreeNode(ba);
        bool ok = lastNode->setNextNodeIfNotSet(textNode);
        assert(ok);
        m_lastSibling = textNode;
    }
}

void VfmdElementTreeNode::adoptContainedElements(VfmdOpeningSpanTagStackNode *stackNode)
{
    VfmdElementTreeNode *containedElements = stackNode->m_containedElements;
    stackNode->m_containedElements = 0;
    appendChildren(containedElements);
}

bool VfmdElementTreeNode::setNextNodeIfNotSet(VfmdElementTreeNode *node)
{
    if (m_nextSibling == 0) {
        m_nextSibling = node;
        return true;
    }
    return false;
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

VfmdElementTreeNode *VfmdElementTreeNode::lastSiblingNode()
{
    if (m_nextSibling == 0) {
        return this;
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
    m_lastSibling = lastSibling;
    return lastSibling;
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

void VfmdElementTreeNode::freeSubtreeSequence(VfmdElementTreeNode *subtree)
{
    const VfmdElementTreeNode *node = subtree;
    while (node != 0) {
        const VfmdElementTreeNode *next = node->nextNode();
        freeSubtreeSequence(node->firstChildNode());
        delete node;
        node = next;
    }
}
