#include "vfmdrenderer.h"
#include "vfmdoutputdevice.h"
#include "vfmdelementtreenodestack.h"

#include <assert.h>

VfmdRenderer::VfmdRenderer()
    : m_outputDevice(0)
{
}

VfmdRenderer::~VfmdRenderer()
{
    delete m_outputDevice;
}

void VfmdRenderer::setOutputDevice(VfmdOutputDevice *outputDevice)
{
    delete m_outputDevice;
    m_outputDevice = outputDevice;
}

VfmdOutputDevice *VfmdRenderer::outputDevice() const
{
    if (m_outputDevice == 0) {
        const_cast<VfmdRenderer*>(this)->m_outputDevice = new VfmdConsoleOutputDevice;
    }
    return m_outputDevice;
}

void VfmdRenderer::render(const VfmdElementTreeNode *parseTree)
{
    beginRendering(parseTree);
    VfmdElementTreeNodeStack ancestorNodes;
    for (const VfmdElementTreeNode *node = parseTree;
         node != 0;
         node = node->nextNode()) {
        renderNode(node, &ancestorNodes);
    }
    endRendering(parseTree);
}

void VfmdRenderer::renderChildNodesOf(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node->hasChildren()) {
        ancestorNodes->push(node);
        for (const VfmdElementTreeNode *childNode = node->firstChildNode();
             childNode != 0;
             childNode = childNode->nextNode()) {
            renderNode(childNode, ancestorNodes);
        }
        const VfmdElementTreeNode *poppedNode = ancestorNodes->pop();
        assert(poppedNode == node);
    }
}

void VfmdRenderer::beginRendering(const VfmdElementTreeNode *parseTree)
{
    UNUSED_ARG(parseTree);
    // Base implementation does nothing
}

bool VfmdRenderer::renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    UNUSED_ARG(node);
    UNUSED_ARG(ancestorNodes);
    // Base implementation does nothing
    return false;
}

void VfmdRenderer::endRendering(const VfmdElementTreeNode *parseTree)
{
    UNUSED_ARG(parseTree);
    // Base implementation does nothing
}
