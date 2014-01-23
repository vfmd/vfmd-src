#ifndef VFMDELEMENTTREENODE_H
#define VFMDELEMENTTREENODE_H

class VfmdElementTreeNode
{
public:
    VfmdElementTreeNode();
    ~VfmdElementTreeNode();

    void addAsLastSiblingNode(VfmdElementTreeNode *node);
    void addAsLastChildNode(VfmdElementTreeNode *node);

    bool hasNext() const;
    bool hasChildren() const;

    VfmdElementTreeNode *nextNode() const;
    VfmdElementTreeNode *firstChildNode() const;

private:
    void insertAsNextNode(VfmdElementTreeNode *node);
    void insertAsFirstChild(VfmdElementTreeNode *node);

    VfmdElementTreeNode *lastSiblingNode() const;

    VfmdElementTreeNode *m_nextSibling, *m_lastSibling;
    VfmdElementTreeNode *m_firstChild;
};

#endif // VFMDELEMENTTREENODE_H
