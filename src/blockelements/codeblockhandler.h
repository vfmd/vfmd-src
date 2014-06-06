#ifndef CODEBLOCKHANDLER_H
#define CODEBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class CodeBlockHandler : public VfmdBlockElementHandler {
public:
    CodeBlockHandler() { }
    virtual bool isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;
    virtual const char *description() const { return "code-block"; }
};

class CodeBlockLineSequence : public VfmdBlockLineSequence {
public:
    CodeBlockLineSequence(const VfmdInputLineSequence *parent);
    virtual int elementType() const { return VfmdConstants::CODE_BLOCK_ELEMENT; }
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual void endBlock();

    VfmdByteArray content() const;

private:
    VfmdByteArray m_content;
    int m_numOfBytesExcludingLastSeenBlankLine;
};

class CodeBlockTreeNode : public VfmdElementTreeNode {
public:
    CodeBlockTreeNode(const VfmdByteArray &content);

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::CODE_BLOCK_ELEMENT; }
    virtual const char *elementTypeString() const { return "code-block"; }

    virtual bool hasTextContent() const { return true; }
    virtual VfmdByteArray textContent() const { return m_content; }

private:
    VfmdByteArray m_content;
};

#endif // CODEBLOCKHANDLER_H
