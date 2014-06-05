#ifndef ORDEREDLISTHANDLER_H
#define ORDEREDLISTHANDLER_H

#include "vfmdblockelementhandler.h"

class OrderedListHandler : public VfmdBlockElementHandler {
public:
    OrderedListHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "ordered-list"; }

private:
    VfmdByteArray m_listStarterString;
    VfmdByteArray m_startingNumber;
};

class OrderedListTreeNode;

class OrderedListLineSequence : public VfmdBlockLineSequence {
public:
    OrderedListLineSequence(const VfmdInputLineSequence *parent,
                            int listStarterStringLength,
                            const VfmdByteArray& startingNumber);
    virtual ~OrderedListLineSequence();
    virtual int elementType() const { return VfmdConstants::ORDERED_LIST_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();

private:
    int m_listStarterStringLength;
    VfmdByteArray m_startingNumber;
    VfmdInputLineSequence *m_childSequence;
    OrderedListTreeNode *m_listNode;
    int m_numOfClosedListItems;
    const VfmdLine *m_previousLine;
    bool m_isCurrentListItemPrecededByABlankLine;
    int m_nextLineListItemStartPrefixLength;
};

class OrderedListTreeNode : public VfmdElementTreeNode {
public:
    OrderedListTreeNode(const VfmdByteArray& startingNumber);

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::ORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "ordered-list"; }

    VfmdByteArray startingNumber() const { return m_startingNumber; }

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

    void setTopPacked(bool yes) { m_isTopPacked = yes; }
    void setBottomPacked(bool yes) { m_isBottomPacked = yes; }
    bool isTopPacked() const { return m_isTopPacked; }
    bool isBottomPacked() const { return m_isBottomPacked; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::ORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "ordered-list-item"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    bool m_isTopPacked, m_isBottomPacked;
};

#endif // ORDEREDLISTHANDLER_H
