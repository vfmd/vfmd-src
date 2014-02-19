#ifndef VFMDINPUTLINESEQUENCE_H
#define VFMDINPUTLINESEQUENCE_H

#include "vfmdline.h"

class VfmdBlockElementHandler;
class VfmdBlockLineSequence;
class VfmdElementRegistry;
class VfmdElementTreeNode;

class VfmdInputLineSequence
{
public:
    VfmdInputLineSequence(const VfmdElementRegistry *registry);

    void addLine(const VfmdLine &line);
    VfmdElementTreeNode* endSequence();

    bool isAtEnd() const;

    VfmdLine currentLine() const;
    VfmdLine nextLine() const;

    const VfmdElementRegistry *registry() const;

    bool hasChildSequence() const;

    // setChildSequence():
    // Sets a VfmdBlockLineSequence as the child of this sequence.
    // The VfmdInputLineSequence owns the child sequence object.
    // This method should be called only from a 'createChildSequence()' method
    // in a class derived from 'VfmdBlockElementHandler'
    void setChildSequence(VfmdBlockLineSequence *lineSequence);

private:
    /* Prevent copying of this class */
    VfmdInputLineSequence(const VfmdInputLineSequence& other);
    VfmdInputLineSequence& operator=(const VfmdInputLineSequence& other);

    void processLineInChildSequence();

    const VfmdElementRegistry *m_registry;
    VfmdLine m_currentLine, m_nextLine;
    bool m_isAtEnd;
    VfmdBlockLineSequence *m_childLineSequence;
    VfmdElementTreeNode *m_parseTree;
};

#endif // VFMDINPUTLINESEQUENCE_H
