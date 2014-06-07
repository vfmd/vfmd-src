#ifndef TREERENDERER_H
#define TREERENDERER_H

#include "renderers/vfmdrenderer.h"

class CodeBlockTreeNode;
class LinkTreeNode;
class EmphasisTreeNode;
class CodeSpanTreeNode;
class ImageTreeNode;
class HtmlTreeNode;
class AutomaticLinkTreeNode;

class TreeRenderer : public VfmdRenderer
{
public:
    TreeRenderer();

    // Output only the structure of the tree, no data (Default: disabled)
    void setStructureOutputOnly(bool yes);
    bool isStructureOutputOnly() const;

protected:

    // Reimplemented
    virtual bool renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Helpers
    void renderTreePrefix(const VfmdElementTreeNodeStack *ancestorNodes, const char *followup);
    void renderTreeText(const VfmdElementTreeNode *node, const VfmdElementTreeNodeStack *ancestorNodes,
                        const VfmdByteArray &text, bool decorate = false);

    // Block elements info
    void renderCodeBlockInfo(const CodeBlockTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Span elements info
    void renderLinkInfo(const LinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderEmphasisInfo(const EmphasisTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderCodeSpanInfo(const CodeSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderImageInfo(const ImageTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderInlinedHtmlInfo(const HtmlTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);
    void renderAutomaticLinkInfo(const AutomaticLinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

    // Text data
    void renderTextSpanInfo(const TextSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes);

private:
    bool m_isStructureOutputOnly;
};

#endif // TREERENDERER_H
