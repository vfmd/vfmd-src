#include "treerenderer.h"

#include "blockelements/codeblockhandler.h"

#include "spanelements/linkhandler.h"
#include "spanelements/emphasishandler.h"
#include "spanelements/codespanhandler.h"
#include "spanelements/imagehandler.h"
#include "spanelements/htmltaghandler.h"
#include "spanelements/automaticlinkhandler.h"
#include "textspantreenode.h"

#include "vfmdelementtreenodestack.h"

TreeRenderer::TreeRenderer()
    : m_isStructureOutputOnly(false)
{
}

void TreeRenderer::setStructureOutputOnly(bool yes)
{
    m_isStructureOutputOnly = yes;
}

bool TreeRenderer::isStructureOutputOnly() const
{
    return m_isStructureOutputOnly;
}

bool TreeRenderer::renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestors)
{
    renderTreePrefix(ancestors, "+- ");
    switch (node->elementClassification()) {
    case VfmdElementTreeNode::BLOCK:
        outputDevice()->write("block (");
        break;
    case VfmdElementTreeNode::SPAN:
    case VfmdElementTreeNode::TEXTSPAN:
        outputDevice()->write("span (");
        break;
    case VfmdElementTreeNode::UNDEFINED:
    default:
        outputDevice()->write("? (");
    }
    outputDevice()->write(node->elementTypeString());
    outputDevice()->write(")\n");

    if (isStructureOutputOnly() == false) {

        if (node->elementClassification() == VfmdElementTreeNode::BLOCK) {

            switch(node->elementType()) {
            case VfmdConstants::CODE_BLOCK_ELEMENT:
                renderCodeBlockInfo(dynamic_cast<const CodeBlockTreeNode *>(node), ancestors);
                break;
            }

        } else if (node->elementClassification() == VfmdElementTreeNode::SPAN) {

            switch(node->elementType()) {
            case VfmdConstants::LINK_ELEMENT:
                renderLinkInfo(dynamic_cast<const LinkTreeNode *>(node), ancestors);
                break;
            case VfmdConstants::EMPHASIS_ELEMENT:
                renderEmphasisInfo(dynamic_cast<const EmphasisTreeNode *>(node), ancestors);
                break;
            case VfmdConstants::CODE_SPAN_ELEMENT:
                renderCodeSpanInfo(dynamic_cast<const CodeSpanTreeNode *>(node), ancestors);
                break;
            case VfmdConstants::IMAGE_ELEMENT:
                renderImageInfo(dynamic_cast<const ImageTreeNode *>(node), ancestors);
                break;
            case VfmdConstants::HTML_ELEMENT:
                renderInlinedHtmlInfo(dynamic_cast<const HtmlTreeNode *>(node), ancestors);
                break;
            case VfmdConstants::AUTOMATIC_LINK_ELEMENT:
                renderAutomaticLinkInfo(dynamic_cast<const AutomaticLinkTreeNode *>(node), ancestors);
                break;
            }

        } else if (node->elementClassification() == VfmdElementTreeNode::TEXTSPAN) {

            assert(node->elementType() == VfmdConstants::TEXTSPAN_ELEMENT);
            renderTextSpanInfo(dynamic_cast<const TextSpanTreeNode *>(node), ancestors);

        }

    } // end of 'if (isStructureOutputOnly() == false)'

    if (node->hasChildren()) {
        renderTreePrefix(ancestors, (node->hasNext()? "|  |\n" : "   |\n"));
        renderChildNodesOf(node, ancestors);
    }
    return true;
}

// Helpers

void TreeRenderer::renderTreePrefix(const VfmdElementTreeNodeStack *ancestorNodes, const char *followup)
{
    unsigned int ancestorsCount = ancestorNodes->size();
    for (unsigned int i = 0; i < ancestorsCount; i++) {
        const VfmdElementTreeNode *ancestorNode = ancestorNodes->nodeAt(i);
        if (ancestorNode->hasNext()) {
            outputDevice()->write("|  ");
        } else {
            outputDevice()->write("   ");
        }
    }
    if (followup) {
        outputDevice()->write(followup);
    }
}

void TreeRenderer::renderTreeText(const VfmdElementTreeNode *node, const VfmdElementTreeNodeStack *ancestorNodes,
                                  const VfmdByteArray &text,
                                  bool decorate)
{
    const char *data_ptr = text.data();
    size_t sz = text.size();
    if (data_ptr && sz) {
        if (decorate) {
            renderTreePrefix(ancestorNodes, (node->hasNext()? "|  \'" : "   \'"));
        } else {
            renderTreePrefix(ancestorNodes, (node->hasNext()? "|  " : "   "));
        }
        for (unsigned int i = 0; i < sz; i++) {
            if (data_ptr[i] == '\n') {
                if (decorate) {
                    outputDevice()->write("\\n\'\n");
                    if (i < (sz - 1)) {
                        renderTreePrefix(ancestorNodes, (node->hasNext()? "|  \'" : "   \'"));
                    }
                } else {
                    outputDevice()->write('\n');
                    if (i < (sz - 1)) {
                        renderTreePrefix(ancestorNodes, (node->hasNext()? "|  " : "   "));
                    }
                }
            } else {
                outputDevice()->write(data_ptr[i]);
            }
        }
        if (text.lastByte() != '\n') {
            outputDevice()->write(decorate? "\'\n" : "\n");
        }
    }
}

// Block elements info

void TreeRenderer::renderCodeBlockInfo(const CodeBlockTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    renderTreeText(node, ancestorNodes, node->textContent(), true);
}

// Span elements info

void TreeRenderer::renderLinkInfo(const LinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    VfmdByteArray ba("[");
    if (node->refType() == LinkTreeNode::NO_REF) {
        ba.append("URL = \"");
        ba.append(node->url());
        ba.append("\"");
    } else {
        ba.append("Ref = \"");
        ba.append(node->refIdText());
        ba.append("\"");
    }
    ba.append("]");
    renderTreeText(node, ancestorNodes, ba);
}

void TreeRenderer::renderEmphasisInfo(const EmphasisTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    VfmdByteArray ba("[");
    ba.appendByteNtimes(node->emphasisCharacter(), node->repetitionCount());
    ba.append("]");
    renderTreeText(node, ancestorNodes, ba);
}

void TreeRenderer::renderCodeSpanInfo(const CodeSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    renderTreeText(node, ancestorNodes, node->textContent(), true);
}

void TreeRenderer::renderImageInfo(const ImageTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    VfmdByteArray ba("[");
    if (node->refType() == ImageTreeNode::NO_REF) {
        ba.append("URL = \"");
        ba.append(node->url());
        ba.append("\"");
    } else {
        ba.append("Ref = \"");
        ba.append(node->refIdText());
        ba.append("\"");
    }
    ba.append("]");
    renderTreeText(node, ancestorNodes, ba);
}

void TreeRenderer::renderInlinedHtmlInfo(const HtmlTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    VfmdByteArray ba("[");
    switch (node->htmlNodeType()) {
    case HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG:
        ba.append("start + end tag (");
        ba.append(node->tagName());
        ba.append(")");
        break;
    case HtmlTreeNode::EMPTY_TAG:
        ba.append("void tag (");
        ba.append(node->tagName());
        ba.append(")");
        break;
    case HtmlTreeNode::START_TAG_ONLY:
        ba.append("start tag only (");
        ba.append(node->tagName());
        ba.append(")");
        break;
    case HtmlTreeNode::END_TAG_ONLY:
        ba.append("end tag only (");
        ba.append(node->tagName());
        ba.append(")");
        break;
    case HtmlTreeNode::COMMENT:
        ba.append("comment");
        break;
    case HtmlTreeNode::VERBATIM_HTML_CHUNK:
        ba.append("verbatim html");
        break;
    default:
        assert(false);
    }
    ba.append("]\n");
    renderTreeText(node, ancestorNodes, ba);

    if (node->htmlNodeType() == HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG) {
        VfmdByteArray ba = node->startTagText();
        ba.append(" ... ");
        ba.append(node->endTagText());
        renderTreeText(node, ancestorNodes, ba, true);
    } else {
        renderTreeText(node, ancestorNodes, node->fullHtmlText(), true);
    }
}

void TreeRenderer::renderAutomaticLinkInfo(const AutomaticLinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    renderTreeText(node, ancestorNodes, node->url());
}

// Text info

void TreeRenderer::renderTextSpanInfo(const TextSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    renderTreeText(node, ancestorNodes, node->textContent(), true);
}
