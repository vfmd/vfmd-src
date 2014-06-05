#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H

#include "vfmdrenderer.h"

class SetextHeaderTreeNode;
class AtxHeaderTreeNode;
class CodeBlockTreeNode;
class BlockquoteTreeNode;
class HorizontalRuleTreeNode;
class UnorderedListTreeNode;
class OrderedListTreeNode;
class LinkTreeNode;
class EmphasisTreeNode;
class CodeSpanTreeNode;
class ImageTreeNode;
class HtmlTreeNode;
class AutomaticLinkTreeNode;
class ParagraphTreeNode;

class HtmlRenderer : public VfmdRenderer
{
public:
    HtmlRenderer();

    // Enable indentation (Default: enabled)
    void setIndentationEnabled(bool yes);
    bool isIndentationEnabled() const;

    // Enable XHTML-compatible void tags output (Default: enabled)
    void setSelfClosingVoidTagsEnabled(bool yes);
    bool isSelfClosingVoidTagsEnabled() const;

    // Enable outputting <br/> tags on every newline (Default: disabled)
    void setLineBreakOnNewlinesEnabled(bool yes);
    bool isLineBreakOnNewlinesEnabled() const;

protected:

    // Reimplemented
    virtual bool renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Helpers
    void renderIndentation(int level);

    // Block elements
    void renderSetextHeader(const SetextHeaderTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderAtxHeader(const AtxHeaderTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderCodeBlock(const CodeBlockTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderBlockquote(const BlockquoteTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderHorizontalRule(const HorizontalRuleTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderUnorderedList(const UnorderedListTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderOrderedList(const OrderedListTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderParagraph(const ParagraphTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Span elements
    void renderLink(const LinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderEmphasis(const EmphasisTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderCodeSpan(const CodeSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderImage(const ImageTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderInlinedHtml(const HtmlTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderAutomaticLink(const AutomaticLinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Text
    void renderTextSpan(const TextSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

private:
    bool m_isIndentationEnabled, m_isSelfClosingVoidTagsEnabled, m_isLineBreakOnNewlinesEnabled;
};

#endif // VFMDHTMLRENDERER_H
