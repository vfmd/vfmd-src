#ifndef CODEBLOCKHANDLER_H
#define CODEBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class CodeBlockHandler : public VfmdBlockElementHandler {
public:
    CodeBlockHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine);
    virtual ~CodeBlockHandler() { }
    virtual const char *description() const { return "code-block"; }
};

class CodeBlockLineSequence : public VfmdBlockLineSequence {
public:
    CodeBlockLineSequence(const VfmdInputLineSequence *parent);
    virtual ~CodeBlockLineSequence() { }
    virtual void processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();

private:
    VfmdByteArray m_content;
    bool m_isAtEnd;
};

class CodeBlockTreeNode : public VfmdElementTreeNode {
public:
    CodeBlockTreeNode(const VfmdByteArray &content);
    ~CodeBlockTreeNode() { }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return BLOCK; }
    virtual int elementType() const { return VfmdConstants::CODE_BLOCK_ELEMENT; }
    virtual const char *elementTypeString() const { return "code-block"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    VfmdByteArray m_content;
};

#endif // CODEBLOCKHANDLER_H
