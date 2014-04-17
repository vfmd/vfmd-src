#ifndef VFMDELEMENTTREENODE_H
#define VFMDELEMENTTREENODE_H

#include "vfmdbytearray.h"
#include "vfmdconstants.h"
#include "vfmdoutputdevice.h"

#define UNUSED_ARG(x) (void)x;

class TextSpanTreeNode;
class VfmdOpeningSpanTagStackNode;
class VfmdOutputDevice;
class VfmdElementTreeNodeStack;

class VfmdElementTreeNode
{
public:
    enum ElementClassification {
        UNDEFINED = -1,
        BLOCK,
        SPAN,
        TEXTSPAN
    };

    VfmdElementTreeNode();
    virtual ~VfmdElementTreeNode();

    void appendSubtreeToEndOfSequence(VfmdElementTreeNode *subtreeToAppend);
    void appendTextToEndOfSequence(const VfmdByteArray &textToAppend);
    void adoptAsLastChild(VfmdElementTreeNode *subtreeToAdopt);

    void adoptContainedElements(VfmdOpeningSpanTagStackNode *stackNode);

    bool hasNext() const;
    bool hasChildren() const;

    VfmdElementTreeNode *nextNode() const;
    VfmdElementTreeNode *firstChildNode() const;
    VfmdElementTreeNode *lastChildNode() const;

    bool setNextNodeIfNotSet(VfmdElementTreeNode *node);
    bool setChildNodeIfNotSet(VfmdElementTreeNode *node);

    // Methods for rendering the output
    void renderSequence(VfmdConstants::RenderFormat format, int renderOptions,
                        VfmdOutputDevice *outputDevice) const;
    void renderChildren(VfmdConstants::RenderFormat format, int renderOptions,
                        VfmdOutputDevice *outputDevice,
                        VfmdElementTreeNodeStack *ancestorNodes) const;
    void renderTreePrefix(VfmdOutputDevice *outputDevice,
                          const VfmdElementTreeNodeStack *ancestorNodes,
                          const char *followup = 0) const;
    void renderTreeText(VfmdOutputDevice *outputDevice,
                        const VfmdElementTreeNodeStack *ancestorNodes,
                        const VfmdByteArray &text) const;
    void renderHtmlIndent(VfmdOutputDevice *outputDevice,
                          const VfmdElementTreeNodeStack *ancestorNodes) const;

    // Methods to reimplement in a subclass
    virtual ElementClassification elementClassification() const { return UNDEFINED; }
    virtual int elementType() const { return -1; }
    virtual const char *elementTypeString() const { return ""; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

    virtual bool hasTextContent() const;
    virtual VfmdByteArray textContent() const;

public:
    static void freeSubtreeSequence(VfmdElementTreeNode *tree);

private:
    /* Prevent copying of this class */
    VfmdElementTreeNode(const VfmdElementTreeNode& other);
    VfmdElementTreeNode& operator=(const VfmdElementTreeNode& other);

    VfmdElementTreeNode *lastSiblingNode();

    VfmdElementTreeNode *m_nextSibling, *m_lastSibling;
    VfmdElementTreeNode *m_firstChild;
};

#endif // VFMDELEMENTTREENODE_H
