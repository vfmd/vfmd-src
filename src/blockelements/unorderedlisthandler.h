#ifndef UNORDEREDLISTHANDLER_H
#define UNORDEREDLISTHANDLER_H

#include "vfmdblockelementhandler.h"

class UnorderedListHandler : public VfmdBlockElementHandler {
public:
    UnorderedListHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~UnorderedListHandler() { }
    virtual const char *description() const { return "unordered-list"; }
};

class UnorderedListTreeNode;

class UnorderedListLineSequence : public VfmdBlockLineSequence {
public:
    UnorderedListLineSequence(const VfmdInputLineSequence *parent, const VfmdByteArray &starterString);
    virtual ~UnorderedListLineSequence();
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

    void closeListItem(bool isEndOfList);
    bool isTopPackedListItem(bool isEndOfList) const;
    bool isBottomPackedListItem(bool isEndOfList) const;

private:
    VfmdByteArray m_listStarterString;
    VfmdInputLineSequence *m_childSequence;
    UnorderedListTreeNode *m_listNode;
    int m_numOfClosedListItems;
    VfmdLine m_previousLine;
    bool m_isCurrentListItemPrecededByABlankLine;
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

    void setTopPacked(bool yes) { m_isTopPacked = yes; }
    void setBottomPacked(bool yes) { m_isBottomPacked = yes; }
    bool isTopPacked() const { return m_isTopPacked; }
    bool isBottomPacked() const { return m_isBottomPacked; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "unordered-list-item"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    bool m_isTopPacked, m_isBottomPacked;
};

#endif // UNORDEREDLISTHANDLER_H
