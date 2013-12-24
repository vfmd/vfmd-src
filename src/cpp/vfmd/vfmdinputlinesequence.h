#ifndef VFMDINPUTLINESEQUENCE_H
#define VFMDINPUTLINESEQUENCE_H

#include "vfmdline.h"

class VfmdBlockSyntaxHandler;
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
    VfmdBlockSyntaxHandler *m_blockSyntaxHandlers[2];
    int m_blockSyntaxHandlerCount;
};

#endif // VFMDINPUTLINESEQUENCE_H
