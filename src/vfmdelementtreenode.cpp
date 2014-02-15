#include <assert.h>
#include <stdio.h>
#include "vfmdelementtreenode.h"
#include "textspantreenode.h"
#include "vfmdspanelementhandler.h"
#include "vfmdelementtreenodestack.h"
#include "vfmdoutputdevice.h"

VfmdElementTreeNode::VfmdElementTreeNode()
    : m_nextSibling(0)
    , m_lastSibling(0)
    , m_firstChild(0)
{
}

VfmdElementTreeNode::~VfmdElementTreeNode()
{
}

void VfmdElementTreeNode::appendSubtreeToEndOfSequence(VfmdElementTreeNode *nodesToAdd)
{
    assert(nodesToAdd);
    VfmdElementTreeNode *lastNode = lastSiblingNode();
    if ((nodesToAdd->elementClassification() == VfmdElementTreeNode::TEXTSPAN) &&
        (lastNode->elementClassification() == VfmdElementTreeNode::TEXTSPAN)) {
        // Do not allow two text nodes side-by-side.
        // Merge them into a single text node.
        TextSpanTreeNode *textNode = dynamic_cast<TextSpanTreeNode *>(nodesToAdd);
        TextSpanTreeNode *textLastNode = dynamic_cast<TextSpanTreeNode *>(lastNode);
        assert(textNode != 0);
        assert(textLastNode != 0);
        textLastNode->appendText(textNode->text());
        bool ok = textLastNode->setNextNodeIfNotSet(nodesToAdd->nextNode());
        assert(ok);
        VfmdElementTreeNode *possibleLastSiblingNode = nodesToAdd->lastSiblingNode();
        if (possibleLastSiblingNode != nodesToAdd) {
            m_lastSibling = possibleLastSiblingNode;
        }
        delete nodesToAdd;
    } else {
        bool ok = lastNode->setNextNodeIfNotSet(nodesToAdd);
        assert(ok);
        m_lastSibling = nodesToAdd->lastSiblingNode();
    }
}

void VfmdElementTreeNode::appendTextToEndOfSubtree(const VfmdByteArray &textToAdd)
{
    VfmdElementTreeNode *lastNode = lastSiblingNode();
    if (lastNode->elementClassification() == VfmdElementTreeNode::TEXTSPAN) {
        TextSpanTreeNode *textLastNode = dynamic_cast<TextSpanTreeNode *>(lastNode);
        assert(textLastNode != 0);
        textLastNode->appendText(textToAdd);
    } else {
        TextSpanTreeNode *textNode = new TextSpanTreeNode(textToAdd);
        bool ok = lastNode->setNextNodeIfNotSet(textNode);
        assert(ok);
        m_lastSibling = textNode;
    }
}

void VfmdElementTreeNode::adoptContainedElements(VfmdOpeningSpanTagStackNode *stackNode)
{
    VfmdElementTreeNode *containedElements = stackNode->m_containedElements;
    if (containedElements) {
        if (m_firstChild == 0) {
            m_firstChild = containedElements;
        } else {
            m_firstChild->appendSubtreeToEndOfSequence(containedElements);
        }
        stackNode->m_containedElements = 0;
    }
}

bool VfmdElementTreeNode::setNextNodeIfNotSet(VfmdElementTreeNode *node)
{
    if (m_nextSibling == 0) {
        m_nextSibling = node;
        return true;
    }
    return false;
}

bool VfmdElementTreeNode::setChildNodeIfNotSet(VfmdElementTreeNode *node)
{
    if (m_firstChild == 0) {
        m_firstChild = node;
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

void VfmdElementTreeNode::renderSequence(VfmdConstants::RenderFormat format, int renderOptions,
                                         VfmdOutputDevice *outputDevice) const
{
    VfmdElementTreeNodeStack ancestorNodes;
    for (const VfmdElementTreeNode *node = this;
         node != 0;
         node = node->nextNode()) {
        node->renderNode(format, renderOptions, outputDevice, &ancestorNodes);
    }
}

void VfmdElementTreeNode::renderChildren(VfmdConstants::RenderFormat format, int renderOptions,
                                         VfmdOutputDevice *outputDevice,
                                         VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (hasChildren()) {
        ancestorNodes->push(this);
        for (const VfmdElementTreeNode *childNode = firstChildNode();
             childNode != 0;
             childNode = childNode->nextNode()) {
            childNode->renderNode(format, renderOptions, outputDevice, ancestorNodes);
        }
        const VfmdElementTreeNode *poppedNode = ancestorNodes->pop();
        assert(poppedNode == this);
    }
}

void VfmdElementTreeNode::renderTreePrefix(VfmdOutputDevice *outputDevice,
                                           const VfmdElementTreeNodeStack *ancestorNodes,
                                           const char *followup) const
{
    unsigned int ancestorsCount = ancestorNodes->size();
    for (unsigned int i = 0; i < ancestorsCount; i++) {
        const VfmdElementTreeNode *ancestorNode = ancestorNodes->nodeAt(i);
        if (ancestorNode->hasNext()) {
            outputDevice->write("|  ");
        } else {
            outputDevice->write("   ");
        }
    }
    if (followup) {
        outputDevice->write(followup);
    }
}

void VfmdElementTreeNode::renderHtmlIndent(VfmdOutputDevice *outputDevice,
                                           const VfmdElementTreeNodeStack *ancestorNodes) const
{
    outputDevice->write(' ', ancestorNodes->size() * 2);
}

void VfmdElementTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                     VfmdOutputDevice *outputDevice,
                                     VfmdElementTreeNodeStack *ancestorNodes) const
{
    UNUSED_ARG(renderOptions);
    if (format == VfmdConstants::TREE_FORMAT) {
        renderTreePrefix(outputDevice, ancestorNodes, "+- ");
        switch (elementClassification()) {
        case BLOCK:
            outputDevice->write("block (");
            break;
        case SPAN:
        case TEXTSPAN:
            outputDevice->write("span (");
            break;
        case UNDEFINED:
        default:
            outputDevice->write("? (");
        }
        outputDevice->write(elementTypeString());
        outputDevice->write(")\n");
        if (hasChildren()) {
            renderTreePrefix(outputDevice, ancestorNodes, (hasNext()? "|  |\n" : "   |\n"));
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        }
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
