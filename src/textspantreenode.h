#ifndef TEXTSPANTREENODE_H
#define TEXTSPANTREENODE_H

#include "vfmdelementtreenode.h"
#include <stdio.h>

class TextSpanTreeNode : public VfmdElementTreeNode
{
public:
    TextSpanTreeNode();
    TextSpanTreeNode(const VfmdByteArray &text);
    TextSpanTreeNode(char *str, int len = 0);

    ~TextSpanTreeNode();

    void appendText(const char *str, int len = 0 /* zero implies null-terminated string */);
    void appendText(const VfmdByteArray &ba);

    // Reimplemented
    virtual ElementClassification elementClassification() const { return TEXTSPAN; }
    virtual int elementType() const { return VfmdConstants::TEXTSPAN_ELEMENT; }
    virtual const char *elementTypeString() const { return "text-span"; }

    virtual bool hasTextContent() const { return true; }
    virtual VfmdByteArray textContent() const { return m_text; }

private:
    VfmdByteArray m_text;
};

#endif // TEXTSPANTREENODE_H
