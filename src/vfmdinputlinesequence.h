#ifndef VFMDINPUTLINESEQUENCE_H
#define VFMDINPUTLINESEQUENCE_H

#include "vfmdline.h"
#include "vfmdconstants.h"

class VfmdBlockElementHandler;
class VfmdBlockLineSequence;
class VfmdElementRegistry;
class VfmdElementTreeNode;
class ParagraphLineSequence;

class VfmdInputLineSequence
{
public:
    VfmdInputLineSequence(const VfmdElementRegistry *registry, const VfmdBlockLineSequence *parentLineSequence = 0);

    void addLine(VfmdLine *line);
    VfmdElementTreeNode* endSequence();

    bool isAtEnd() const;

    const VfmdBlockLineSequence *parentLineSequence() const;
    const VfmdElementRegistry *registry() const;

    int containingBlockType() const;
    bool hasChildSequence() const;

    // setChildSequence():
    // Sets a VfmdBlockLineSequence as the child of this sequence.
    // The VfmdInputLineSequence owns the child sequence object.
    // This method should be called only from a 'createChildSequence()' method
    // in a class derived from 'VfmdBlockElementHandler'
    void setChildSequence(VfmdBlockLineSequence *lineSequenceD);

    // addToParseTree():
    // Add a parse-subtree to the end of the parse tree held
    // by this sequence.
    // The VfmdInputLineSequence owns the subtree.
    void addToParseTree(VfmdElementTreeNode *subtreeD);

private:
    /* Prevent copying of this class */
    VfmdInputLineSequence(const VfmdInputLineSequence& other);
    VfmdInputLineSequence& operator=(const VfmdInputLineSequence& other);

    void processInChildSequence(const VfmdLine *currentLine, const VfmdLine *nextLine);

    const VfmdBlockLineSequence *m_parentLineSequence;
    const int m_containingBlockType;
    const VfmdElementRegistry *m_registry;
    const VfmdLine *m_nextLine;
    VfmdBlockLineSequence *m_childLineSequence;
    ParagraphLineSequence *m_paragraphLineSequence;
    int m_numOfLinesGivenToChildLineSequence;
    VfmdBlockElementHandler *m_nextBlockHandler;
    VfmdElementTreeNode *m_parseTree;
};

#endif // VFMDINPUTLINESEQUENCE_H
