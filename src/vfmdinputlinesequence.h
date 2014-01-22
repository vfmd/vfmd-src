#ifndef VFMDINPUTLINESEQUENCE_H
#define VFMDINPUTLINESEQUENCE_H

#include "vfmdline.h"

class VfmdBlockElementHandler;
class VfmdBlockLineSequence;
class VfmdElementRegistry;

class VfmdInputLineSequence
{
public:
    VfmdInputLineSequence(const VfmdElementRegistry *registry);

    void addLine(const VfmdLine &line);
    void endSequence();

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
    void processLineInChildSequence();

    VfmdLine m_currentLine, m_nextLine;
    bool m_isAtEnd;
    const VfmdElementRegistry *m_registry;
    VfmdBlockLineSequence *m_childLineSequence;
};

#endif // VFMDINPUTLINESEQUENCE_H
