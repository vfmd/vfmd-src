#ifndef UNORDEREDLISTHANDLER_H
#define UNORDEREDLISTHANDLER_H

#include "vfmdblockelementhandler.h"

class UnorderedListHandler : public VfmdBlockElementHandler {
public:
    UnorderedListHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~UnorderedListHandler() { }
    virtual const char *description() const { return "unordered-list"; }
};

class UnorderedListTreeNode;

class UnorderedListLineSequence : public VfmdBlockLineSequence {
public:
    UnorderedListLineSequence(const VfmdInputLineSequence *parent, const VfmdByteArray &starterString);
    virtual ~UnorderedListLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

    void closeListItem();

private:
    VfmdByteArray m_listStarterString;
    VfmdInputLineSequence *m_childSequence;
    UnorderedListTreeNode *m_listNode;
};

class UnorderedListTreeNode : public VfmdElementTreeNode {
public:
    UnorderedListTreeNode() { }
    ~UnorderedListTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "unordered-list"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

class UnorderedListItemTreeNode : public VfmdElementTreeNode {
public:
    UnorderedListItemTreeNode() { }
    ~UnorderedListItemTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "unordered-list-item"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
};

#endif // UNORDEREDLISTHANDLER_H
