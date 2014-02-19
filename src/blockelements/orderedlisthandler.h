#ifndef ORDEREDLISTHANDLER_H
#define ORDEREDLISTHANDLER_H

#include "vfmdblockelementhandler.h"

class OrderedListHandler : public VfmdBlockElementHandler {
public:
    OrderedListHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~OrderedListHandler() { }
    virtual const char *description() const { return "ordered-list"; }
};

class OrderedListTreeNode;

class OrderedListLineSequence : public VfmdBlockLineSequence {
public:
    OrderedListLineSequence(const VfmdInputLineSequence *parent,
                            int listStarterStringLength,
                            const VfmdByteArray& startingNumber);
    virtual ~OrderedListLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

    void closeListItem();

private:
    int m_listStarterStringLength;
    VfmdByteArray m_startingNumber;
    VfmdInputLineSequence *m_childSequence;
    OrderedListTreeNode *m_listNode;
};

class OrderedListTreeNode : public VfmdElementTreeNode {
public:
    OrderedListTreeNode(const VfmdByteArray& startingNumber);
    ~OrderedListTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::ORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "ordered-list"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    VfmdByteArray m_startingNumber;
};

class OrderedListItemTreeNode : public VfmdElementTreeNode {
public:
    OrderedListItemTreeNode() { }
    ~OrderedListItemTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::ORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "ordered-list-item"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

#endif // ORDEREDLISTHANDLER_H
