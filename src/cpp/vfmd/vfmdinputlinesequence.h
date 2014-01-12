#ifndef VFMDINPUTLINESEQUENCE_H
#define VFMDINPUTLINESEQUENCE_H

#include "vfmdline.h"

class VfmdBlockElementHandler;
class VfmdBlockLineSequence;

class VfmdInputLineSequence
{
public:
    VfmdInputLineSequence();

    void addLine(const VfmdLine &line);
    void endSequence();

    bool isAtEnd() const;

    VfmdLine currentLine() const;
    VfmdLine nextLine() const;

private:
    void processLineInChildSequence();

    VfmdLine m_currentLine, m_nextLine;
    bool m_isAtEnd;
    VfmdBlockLineSequence *m_childLineSequence;
    VfmdBlockElementHandler *m_blockElementHandlers[2];
    int m_blockSyntaxHandlerCount;
};

#endif // VFMDINPUTLINESEQUENCE_H
