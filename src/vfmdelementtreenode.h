#ifndef VFMDELEMENTTREENODE_H
#define VFMDELEMENTTREENODE_H

#include "vfmdbytearray.h"
#include "vfmdconstants.h"

#define UNUSED_ARG(x) (void)x;

class TextSpanTreeNode;
class VfmdOpeningSpanTagStackNode;

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

    void appendSiblings(VfmdElementTreeNode *node);
    void appendChildren(VfmdElementTreeNode *node);
    void appendText(const VfmdByteArray &ba);
    void adoptContainedElements(VfmdOpeningSpanTagStackNode *stackNode);

    bool hasNext() const;
    bool hasChildren() const;

    VfmdElementTreeNode *nextNode() const;
    VfmdElementTreeNode *firstChildNode() const;

    // Methods to reimplement in a subclass
    virtual ElementClassification elementClassification() const { return UNDEFINED; }
    virtual int elementType() const { return -1; }
    virtual const char *elementTypeString() const { return ""; }
    virtual void debugPrint(const VfmdByteArray &padding) const { UNUSED_ARG(padding); }

public:
    static void debugPrintSubtreeSequence(VfmdElementTreeNode *tree, const VfmdByteArray &padding = VfmdByteArray());
    static void freeSubtreeSequence(VfmdElementTreeNode *tree);

private:
    bool setNextNodeIfNotSet(VfmdElementTreeNode *node);

    VfmdElementTreeNode *lastSiblingNode();

    VfmdElementTreeNode *m_nextSibling, *m_lastSibling;
    VfmdElementTreeNode *m_firstChild;
};

#endif // VFMDELEMENTTREENODE_H
