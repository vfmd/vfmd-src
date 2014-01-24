#ifndef VFMDELEMENTTREENODE_H
#define VFMDELEMENTTREENODE_H

#include "vfmdbytearray.h"
#include "vfmdconstants.h"

#define UNUSED_ARG(x) (void)x;

class TextSpanTreeNode;

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

    void addAsLastSiblingNode(VfmdElementTreeNode *node);
    void addAsLastChildNode(VfmdElementTreeNode *node);
    void setChildSubtree(VfmdElementTreeNode *node);

    bool hasNext() const;
    bool hasChildren() const;

    VfmdElementTreeNode *nextNode() const;
    VfmdElementTreeNode *firstChildNode() const;

    void printSubtreeSequence(const VfmdByteArray &padding = VfmdByteArray()) const;
    void printSubtree(const VfmdByteArray &padding) const;

    // Methods to reimplement in a subclass
    virtual ElementClassification elementClassification() const { return UNDEFINED; }
    virtual int elementType() const { return -1; }
    virtual const char *elementTypeString() const { return ""; }

    // appendEquivalentTextToTextSpanNode() is
    // only applicable for opening-tag span nodes
    virtual void appendEquivalentTextToTextSpanNode(TextSpanTreeNode *node);

public:
    static void freeSubtreeSequence(VfmdElementTreeNode *tree);

private:
    void insertAsNextNode(VfmdElementTreeNode *node);
    void insertAsFirstChild(VfmdElementTreeNode *node);

    VfmdElementTreeNode *lastSiblingNode() const;

    VfmdElementTreeNode *m_nextSibling, *m_lastSibling;
    VfmdElementTreeNode *m_firstChild;
};

#endif // VFMDELEMENTTREENODE_H
