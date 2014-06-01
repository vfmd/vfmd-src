#ifndef VFMDLISTUTILS_H
#define VFMDLISTUTILS_H

#include <assert.h>
#include "blockelements/unorderedlisthandler.h"
#include "blockelements/orderedlisthandler.h"

static inline bool isTopPackedListItem(bool isFirstListItem, bool isLastListItem,
                                       const VfmdLine *lastLineOfListItem,
                                       bool isCurrentListItemPrecededByABlankLine)
{
    if (isFirstListItem && isLastListItem) {
        return true;
    } else if (isFirstListItem && !lastLineOfListItem->isBlankLine()) {
        return true;
    } else if (!isFirstListItem && !isCurrentListItemPrecededByABlankLine) {
        return true;
    }
    return false;
}

static inline bool isBottomPackedListItem(bool isFirstListItem, bool isLastListItem,
                                          const VfmdLine *lastLineOfListItem,
                                          bool isCurrentListItemPrecededByABlankLine)
{
    if (isFirstListItem && isLastListItem) {
        return true;
    } else if (isLastListItem && !isCurrentListItemPrecededByABlankLine) {
        return true;
    } else if (!isLastListItem && !lastLineOfListItem->isBlankLine()) {
        return true;
    }
    return false;
}

template<class ListItemTreeNode>
ListItemTreeNode* closeListItem(VfmdInputLineSequence **childSequence,
                 bool isFirstListItem, bool isLastListItem,
                 const VfmdLine *lastLineOfListItem,
                 bool isCurrentListItemPrecededByABlankLine)
{
    ListItemTreeNode *listItemNode = new ListItemTreeNode();
    listItemNode->setTopPacked(isTopPackedListItem(isFirstListItem, isLastListItem,
                                                   lastLineOfListItem, isCurrentListItemPrecededByABlankLine));
    listItemNode->setBottomPacked(isBottomPackedListItem(isFirstListItem, isLastListItem,
                                                         lastLineOfListItem, isCurrentListItemPrecededByABlankLine));
    assert(childSequence);
    assert(*childSequence);
    VfmdElementTreeNode *childSubTree = (*childSequence)->endSequence();
    bool ok = listItemNode->setChildNodeIfNotSet(childSubTree);
    assert(ok);
    delete (*childSequence);
    (*childSequence) = 0;
    return listItemNode;
}


#endif // VFMDLISTUTILS_H
