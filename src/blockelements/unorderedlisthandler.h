#ifndef UNORDEREDLISTHANDLER_H
#define UNORDEREDLISTHANDLER_H

#include "vfmdblockelementhandler.h"

class UnorderedListHandler : public VfmdBlockElementHandler {
public:
    UnorderedListHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, int containingBlockType, bool isAbuttingParagraph);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "unordered-list"; }

private:
    VfmdByteArray m_listStarterString;
};

class UnorderedListTreeNode;

class UnorderedListLineSequence : public VfmdBlockLineSequence {
public:
    UnorderedListLineSequence(const VfmdInputLineSequence *parent, const VfmdByteArray &starterString);
    virtual ~UnorderedListLineSequence();
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();

private:
    VfmdByteArray m_listStarterString;
    VfmdInputLineSequence *m_childSequence;
    UnorderedListTreeNode *m_listNode;
    int m_numOfClosedListItems;
    const VfmdLine *m_previousLine;
    bool m_isCurrentListItemPrecededByABlankLine;
    bool m_nextLineStartsWithListStarterString;
};

class UnorderedListTreeNode : public VfmdElementTreeNode {
public:
    UnorderedListTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::UNORDERED_LIST_ELEMENT; }
    virtual const char *elementTypeString() const { return "unordered-list"; }
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

private:
    bool m_isTopPacked, m_isBottomPacked;
};

#endif // UNORDEREDLISTHANDLER_H
