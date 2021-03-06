#ifndef CODESPANHANDLER_H
#define CODESPANHANDLER_H

#include "vfmdspanelementhandler.h"

class CodeSpanHandler : public VfmdSpanElementHandler
{
public:
    CodeSpanHandler();
    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                          int currentPos,
                                          VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "code-span"; }
};

class CodeSpanTreeNode : public VfmdElementTreeNode {
public:
    CodeSpanTreeNode(const VfmdByteArray& content) : m_content(content) { }

    VfmdByteArray textContent() const { return m_content; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::CODE_SPAN_ELEMENT; }
    virtual const char *elementTypeString() const { return "code-span"; }

private:
    VfmdByteArray m_content;
};

#endif // CODESPANHANDLER_H
