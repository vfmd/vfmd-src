#ifndef VFMDRENDERER_H
#define VFMDRENDERER_H

#include "vfmdelementtreenode.h"

class VfmdOutputDevice;

class VfmdRenderer
{
public:
    VfmdRenderer();
    virtual ~VfmdRenderer();

    void setOutputDevice(VfmdOutputDevice *outputDevice);
    VfmdOutputDevice *outputDevice() const;

    void render(const VfmdElementTreeNode *parseTree);

protected:
    void renderChildNodesOf(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    virtual void beginRendering(const VfmdElementTreeNode *parseTree);
    virtual bool renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    virtual void endRendering(const VfmdElementTreeNode *parseTree);

private:
    VfmdOutputDevice *m_outputDevice;
};

#endif // VFMDRENDERER_H
