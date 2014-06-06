#include "htmlrenderer.h"
#include "vfmdconstants.h"
#include "vfmdelementtreenodestack.h"
#include "core/htmltextrenderer.h"

#include "blockelements/setextheaderhandler.h"
#include "blockelements/codeblockhandler.h"
#include "blockelements/atxheaderhandler.h"
#include "blockelements/blockquotehandler.h"
#include "blockelements/horizontalrulehandler.h"
#include "blockelements/unorderedlisthandler.h"
#include "blockelements/orderedlisthandler.h"
#include "blockelements/paragraphhandler.h"

#include "spanelements/linkhandler.h"
#include "spanelements/emphasishandler.h"
#include "spanelements/codespanhandler.h"
#include "spanelements/imagehandler.h"
#include "spanelements/htmltaghandler.h"
#include "spanelements/automaticlinkhandler.h"
#include "textspantreenode.h"

HtmlRenderer::HtmlRenderer()
    : m_isIndentationEnabled(true)
    , m_isSelfClosingVoidTagsEnabled(true)
    , m_isLineBreakOnNewlinesEnabled(false)
{
}

void HtmlRenderer::setIndentationEnabled(bool yes)
{
    m_isIndentationEnabled = yes;
}

bool HtmlRenderer::isIndentationEnabled() const
{
    return m_isIndentationEnabled;
}

void HtmlRenderer::setSelfClosingVoidTagsEnabled(bool yes)
{
    m_isSelfClosingVoidTagsEnabled = yes;
}

bool HtmlRenderer::isSelfClosingVoidTagsEnabled() const
{
    return m_isSelfClosingVoidTagsEnabled;
}

void HtmlRenderer::setLineBreakOnNewlinesEnabled(bool yes)
{
    m_isLineBreakOnNewlinesEnabled = yes;
}

bool HtmlRenderer::isLineBreakOnNewlinesEnabled() const
{
    return m_isLineBreakOnNewlinesEnabled;
}

bool HtmlRenderer::renderNode(const VfmdElementTreeNode *node, VfmdElementTreeNodeStack *ancestors)
{
    if (node->elementClassification() == VfmdElementTreeNode::BLOCK) {

        switch(node->elementType()) {
        case VfmdConstants::SETEXT_HEADER_ELEMENT:
            renderSetextHeader(dynamic_cast<const SetextHeaderTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::ATX_HEADER_ELEMENT:
            renderAtxHeader(dynamic_cast<const AtxHeaderTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::CODE_BLOCK_ELEMENT:
            renderCodeBlock(dynamic_cast<const CodeBlockTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::BLOCKQUOTE_ELEMENT:
            renderBlockquote(dynamic_cast<const BlockquoteTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::HORIZONTAL_RULE_ELEMENT:
            renderHorizontalRule(dynamic_cast<const HorizontalRuleTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::UNORDERED_LIST_ELEMENT:
            renderUnorderedList(dynamic_cast<const UnorderedListTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::ORDERED_LIST_ELEMENT:
            renderOrderedList(dynamic_cast<const OrderedListTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::PARAGRAPH_ELEMENT:
            renderParagraph(dynamic_cast<const ParagraphTreeNode *>(node), ancestors);
            break;
        default:
            return false;
        }

    } else if (node->elementClassification() == VfmdElementTreeNode::SPAN) {

        switch(node->elementType()) {
        case VfmdConstants::LINK_ELEMENT:
            renderLink(dynamic_cast<const LinkTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::EMPHASIS_ELEMENT:
            renderEmphasis(dynamic_cast<const EmphasisTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::CODE_SPAN_ELEMENT:
            renderCodeSpan(dynamic_cast<const CodeSpanTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::IMAGE_ELEMENT:
            renderImage(dynamic_cast<const ImageTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::HTML_ELEMENT:
            renderInlinedHtml(dynamic_cast<const HtmlTreeNode *>(node), ancestors);
            break;
        case VfmdConstants::AUTOMATIC_LINK_ELEMENT:
            renderAutomaticLink(dynamic_cast<const AutomaticLinkTreeNode *>(node), ancestors);
            break;
        default:
            return false;
        }

    } else if (node->elementClassification() == VfmdElementTreeNode::TEXTSPAN) {

        assert(node->elementType() == VfmdConstants::TEXTSPAN_ELEMENT);
        renderTextSpan(dynamic_cast<const TextSpanTreeNode *>(node), ancestors);

    } else {
        return false;
    }

    return true;
}

// Helpers

void HtmlRenderer::renderIndentation(int level)
{
    if (isIndentationEnabled()) {
        outputDevice()->write(' ', level * 2);
    }
}

// Block elements

void HtmlRenderer::renderSetextHeader(const SetextHeaderTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    renderIndentation(ancestorNodes->size());

    int headingLevel = node->headingLevel();
    assert(headingLevel == 1 || headingLevel == 2);

    if (headingLevel == 1 || headingLevel == 2) {
        outputDevice()->write((headingLevel == 1)? "<h1>" : "<h2>");
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write((headingLevel == 1)? "</h1>\n" : "</h2>\n");
    }
}

void HtmlRenderer::renderAtxHeader(const AtxHeaderTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    renderIndentation(ancestorNodes->size());

    int headingLevel = node->headingLevel();
    assert(headingLevel >= 1);
    assert(headingLevel <= 6);

    if (headingLevel >= 1 && headingLevel <= 6) {
        outputDevice()->write("<h", 2);
        outputDevice()->write('0' + headingLevel);
        outputDevice()->write('>');
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write("</h", 3);
        outputDevice()->write('0' + headingLevel);
        outputDevice()->write(">\n");
    }
}

void HtmlRenderer::renderCodeBlock(const CodeBlockTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    UNUSED_ARG(ancestorNodes);

    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    outputDevice()->write("<pre><code>", 11);
    HtmlTextRenderer::renderCode(outputDevice(), node->textContent());
    outputDevice()->write("</code></pre>\n", 14);
}

void HtmlRenderer::renderBlockquote(const BlockquoteTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    renderIndentation(ancestorNodes->size());
    outputDevice()->write("<blockquote>\n", 13);
    renderChildNodesOf(node, ancestorNodes);
    renderIndentation(ancestorNodes->size());
    outputDevice()->write("</blockquote>\n", 14);
}

void HtmlRenderer::renderHorizontalRule(const HorizontalRuleTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    renderIndentation(ancestorNodes->size());
    outputDevice()->write(isSelfClosingVoidTagsEnabled()? "<hr />\n" : "<hr>\n");
}

void HtmlRenderer::renderUnorderedList(const UnorderedListTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    outputDevice()->write('\n');
    renderIndentation(ancestorNodes->size());
    outputDevice()->write("<ul>\n", 5);

    if (node->hasChildren()) {
        ancestorNodes->push(node);

        for (const VfmdElementTreeNode *childNode = node->firstChildNode();
             childNode != 0;
             childNode = childNode->nextNode()) {

            const UnorderedListItemTreeNode *listItemNode = dynamic_cast<const UnorderedListItemTreeNode *>(childNode);
            if (listItemNode) {
                renderIndentation(ancestorNodes->size());
                bool containsASingleParagraph = (listItemNode->firstChildNode()->elementType() == VfmdConstants::PARAGRAPH_ELEMENT &&
                                                 listItemNode->firstChildNode()->nextNode() == 0);
                if (containsASingleParagraph) {
                    outputDevice()->write("<li>", 4);
                } else {
                    outputDevice()->write("<li>\n", 5);
                }
                renderChildNodesOf(listItemNode, ancestorNodes);
                if (!containsASingleParagraph) {
                    renderIndentation(ancestorNodes->size());
                }
                outputDevice()->write("</li>\n", 6);
            }

        }

        const VfmdElementTreeNode *poppedNode = ancestorNodes->pop();
        assert(poppedNode == node);
    }

    renderIndentation(ancestorNodes->size());
    outputDevice()->write("</ul>\n", 6);
}

void HtmlRenderer::renderOrderedList(const OrderedListTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    outputDevice()->write('\n');
    renderIndentation(ancestorNodes->size());

    VfmdByteArray startingNumber = node->startingNumber();
    if (startingNumber.size() > 0 && !startingNumber.isEqualTo("1")) {
        outputDevice()->write("<ol start=\"", 11);
        outputDevice()->write(startingNumber);
        outputDevice()->write("\">\n", 3);
    } else {
        outputDevice()->write("<ol>\n", 5);
    }

    if (node->hasChildren()) {
        ancestorNodes->push(node);

        for (const VfmdElementTreeNode *childNode = node->firstChildNode();
             childNode != 0;
             childNode = childNode->nextNode()) {

            const OrderedListItemTreeNode *listItemNode = dynamic_cast<const OrderedListItemTreeNode *>(childNode);
            if (listItemNode) {
                renderIndentation(ancestorNodes->size());
                bool containsASingleParagraph = (listItemNode->firstChildNode()->elementType() == VfmdConstants::PARAGRAPH_ELEMENT &&
                                                 listItemNode->firstChildNode()->nextNode() == 0);
                if (containsASingleParagraph) {
                    outputDevice()->write("<li>", 4);
                } else {
                    outputDevice()->write("<li>\n", 5);
                }
                renderChildNodesOf(listItemNode, ancestorNodes);
                if (!containsASingleParagraph) {
                    renderIndentation(ancestorNodes->size());
                }
                outputDevice()->write("</li>\n", 6);
            }

        }

        const VfmdElementTreeNode *poppedNode = ancestorNodes->pop();
        assert(poppedNode == node);
    }

    renderIndentation(ancestorNodes->size());
    outputDevice()->write("</ol>\n", 6);
}

void HtmlRenderer::renderParagraph(const ParagraphTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    bool canEncloseContentInPTags = (node->shouldAvoidWrappingInHtmlPTag() == false);
    bool isSoleContentOfAListItem = false;
    bool isContainedInTopPackedListItem = false;
    bool isContainedInBottomPackedListItem = false;
    const VfmdElementTreeNode *parentNode = ancestorNodes->topNode();
    if (parentNode) {
        if (parentNode->elementType() == VfmdConstants::UNORDERED_LIST_ELEMENT) {
            const UnorderedListItemTreeNode *listItemNode = dynamic_cast<const UnorderedListItemTreeNode *>(parentNode);
            if (listItemNode) {
                isSoleContentOfAListItem = ((listItemNode->firstChildNode() == node) && (node->nextNode() == 0));
                isContainedInTopPackedListItem = listItemNode->isTopPacked();
                isContainedInBottomPackedListItem = listItemNode->isBottomPacked();
            }
        } else if (parentNode->elementType() == VfmdConstants::ORDERED_LIST_ELEMENT) {
            const OrderedListItemTreeNode *listItemNode = dynamic_cast<const OrderedListItemTreeNode *>(parentNode);
            if (listItemNode) {
                isSoleContentOfAListItem = ((listItemNode->firstChildNode() == node) && (node->nextNode() == 0));
                isContainedInTopPackedListItem = listItemNode->isTopPacked();
                isContainedInBottomPackedListItem = listItemNode->isBottomPacked();
            }
        }
    }

    if (canEncloseContentInPTags) {
        // If this paragraph is part of a list item, there are additional things to be
        // considered in deciding whether the content can be enclosed in p tags
        if (isContainedInTopPackedListItem) {
            bool firstBlockInParent = (parentNode && parentNode->firstChildNode() == node);
            if (firstBlockInParent) {
                canEncloseContentInPTags = false;
            }
        }
        if (isContainedInBottomPackedListItem) {
            bool secondBlockInParent = (parentNode && parentNode->firstChildNode() &&
                                        parentNode->firstChildNode()->nextNode() == node);
            bool lastBlockInParent = (parentNode && parentNode->lastChildNode() == node);
            if (lastBlockInParent && !secondBlockInParent) {
                canEncloseContentInPTags = false;
            }
        }
    }

    if (isSoleContentOfAListItem) {
        // The paragraph is going to be on the same line as the <li> tag.
        // Compact form.
        if (canEncloseContentInPTags) {
            outputDevice()->write("<p>", 3);
            renderChildNodesOf(node, ancestorNodes);
            outputDevice()->write("</p>", 4);
        } else {
            renderChildNodesOf(node, ancestorNodes);
        }
    } else {
        // Normal form.
        renderIndentation(ancestorNodes->size());
        if (canEncloseContentInPTags) {
            outputDevice()->write("<p>", 3);
            renderChildNodesOf(node, ancestorNodes);
            outputDevice()->write("</p>\n\n", 6);
        } else {
            renderChildNodesOf(node, ancestorNodes);
            outputDevice()->write("\n\n", 2);
        }
    }
}

// Span elements

static void renderLinkOpenTag(const VfmdByteArray &url, const VfmdByteArray &title, VfmdOutputDevice *out)
{
    out->write("<a href=\"", 9);
    HtmlTextRenderer::renderURL(out, url);
    if (title.isValid()) {
        out->write("\" title=\"", 9);
        HtmlTextRenderer::renderTagAttribute(out, title);
    }
    out->write("\">", 2);
}

void HtmlRenderer::renderLink(const LinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    if (node->refType() == LinkTreeNode::NO_REF) {
        // Inline link like "[link](url)"
        renderLinkOpenTag(node->url(), node->title(), outputDevice());
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write("</a>", 4);
    } else {
        // Reference link like "[link-ref]" or "[link][ref]" or "[link-ref][]"
        const VfmdLinkRefMap *linkRefMap = node->linkRefMap();
        VfmdByteArray refId = node->refIdText().simplified().toLowerCase();
        if ((refId.size() > 0) && (linkRefMap->hasData(refId))) {
            // We found a definition for this ref id
            VfmdByteArray linkUrl = linkRefMap->linkUrl(refId);
            VfmdByteArray linkTitle = linkRefMap->linkTitle(refId);
            renderLinkOpenTag(linkUrl, linkTitle, outputDevice());
            renderChildNodesOf(node, ancestorNodes);
            outputDevice()->write("</a>", 4);
        } else {
            // We didn't find a definition for this ref id
            outputDevice()->write('[');
            renderChildNodesOf(node, ancestorNodes);
            HtmlTextRenderer::renderText(outputDevice(), node->closeTagText(),
                                         isSelfClosingVoidTagsEnabled(),
                                         isLineBreakOnNewlinesEnabled());
        }
    }
}

void HtmlRenderer::renderEmphasis(const EmphasisTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    int repetitionCount = node->repetitionCount();
    if (repetitionCount == 1) {
        outputDevice()->write("<em>", 4);
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write("</em>", 5);
    } else if (repetitionCount == 2) {
        outputDevice()->write("<strong>", 8);
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write("</strong>", 9);
    } else if (repetitionCount > 2) {
        outputDevice()->write("<strong><em>", 12);
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write("</em></strong>", 14);
    }
}

void HtmlRenderer::renderCodeSpan(const CodeSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    UNUSED_ARG(ancestorNodes);

    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    outputDevice()->write("<code>", 6);
    HtmlTextRenderer::renderCode(outputDevice(), node->textContent().trimmed());
    outputDevice()->write("</code>", 7);
}


static void renderImageTag(const VfmdByteArray &url, const VfmdByteArray &title, const VfmdByteArray &altText,
                           VfmdOutputDevice *out, bool useSelfClosingVoidTags)
{
    out->write("<img src=\"", 10);
    HtmlTextRenderer::renderURL(out, url);
    if (altText.isValid()) {
        out->write("\" alt=\"", 7);
        HtmlTextRenderer::renderTagAttribute(out, altText);
    }
    if (title.isValid()) {
        out->write("\" title=\"", 9);
        HtmlTextRenderer::renderTagAttribute(out, title);
    }
    if (useSelfClosingVoidTags) {
        out->write("\" />", 4);
    } else {
        out->write("\">", 2);
    }
}

void HtmlRenderer::renderImage(const ImageTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    UNUSED_ARG(ancestorNodes);

    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    if (node->refType() == ImageTreeNode::NO_REF) {
        // Inline image like "![img](url)"
        renderImageTag(node->url(), node->title(), node->altText(), outputDevice(), isSelfClosingVoidTagsEnabled());
    } else {
        // Reference image like "![img][ref]" or "![img-ref]" or "![img-ref][]"
        const VfmdLinkRefMap *linkRefMap = node->linkRefMap();
        VfmdByteArray refId = node->refIdText().simplified().toLowerCase();
        if ((refId.size() > 0) && (linkRefMap->hasData(refId))) {
            // We found a definition for this ref id
            VfmdByteArray url = linkRefMap->linkUrl(refId);
            VfmdByteArray title = linkRefMap->linkTitle(refId);
            renderImageTag(url, title, node->altText(), outputDevice(), isSelfClosingVoidTagsEnabled());
        } else {
            // We didn't find a definition for this ref id
            outputDevice()->write("![", 2);
            outputDevice()->write(node->altText());
            HtmlTextRenderer::renderText(outputDevice(), node->closeTagText(),
                                         isSelfClosingVoidTagsEnabled(),
                                         isLineBreakOnNewlinesEnabled());
        }
    }
}

void HtmlRenderer::renderInlinedHtml(const HtmlTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    assert(node->htmlNodeType() == HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG ||
           node->hasChildren() == false);

    switch (node->htmlNodeType()) {
    case HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG:
        outputDevice()->write(node->startTagText());
        renderChildNodesOf(node, ancestorNodes);
        outputDevice()->write(node->endTagText());
        break;
    case HtmlTreeNode::EMPTY_TAG:
    case HtmlTreeNode::START_TAG_ONLY:
    case HtmlTreeNode::END_TAG_ONLY:
    case HtmlTreeNode::COMMENT:
        outputDevice()->write(node->fullHtmlText());
        break;
    case HtmlTreeNode::VERBATIM_HTML_CHUNK:
        outputDevice()->write(node->fullHtmlText());
        break;
    default:
        assert(false);
    }
}

void HtmlRenderer::renderAutomaticLink(const AutomaticLinkTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    outputDevice()->write("<a href=\"", 9);
    if (node->urlType() == AutomaticLinkTreeNode::MAIL_URL_WITHOUT_MAILTO) {
        outputDevice()->write("mailto:", 7);
    }
    HtmlTextRenderer::renderURL(outputDevice(), node->url());
    outputDevice()->write("\">", 2);
    HtmlTextRenderer::renderURLAsText(outputDevice(), node->url());
    outputDevice()->write("</a>", 4);
}

void HtmlRenderer::renderTextSpan(const TextSpanTreeNode *node, VfmdElementTreeNodeStack *ancestorNodes)
{
    UNUSED_ARG(ancestorNodes);

    if (node == 0) {
        return;
    }

    assert(node->hasChildren() == false);

    HtmlTextRenderer::renderText(outputDevice(), node->textContent(),
                                 isSelfClosingVoidTagsEnabled(),
                                 isLineBreakOnNewlinesEnabled());
}
