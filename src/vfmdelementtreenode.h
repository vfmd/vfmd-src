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

    // Methods to reimplement in a subclass
    virtual ElementClassification elementClassification() const { return UNDEFINED; }
    virtual int elementType() const { return -1; }
    virtual const char *elementTypeString() const { return ""; }

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
