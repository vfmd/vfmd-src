#include <string.h>
#include "textspantreenode.h"
#include "vfmdoutputdevice.h"
#include "core/htmltextrenderer.h"

TextSpanTreeNode::TextSpanTreeNode()
{
}

TextSpanTreeNode::TextSpanTreeNode(const VfmdByteArray &text)
    : m_text(text)
{
}

TextSpanTreeNode::TextSpanTreeNode(char *str, int len)
{
    appendText(str, len);
}

TextSpanTreeNode::~TextSpanTreeNode()
{
}

void TextSpanTreeNode::appendText(const char *str, int len)
{
    if (len <= 0) {
        len = strlen(str);
    }
    m_text.append(str, len);
}

void TextSpanTreeNode::appendText(const VfmdByteArray &ba)
{
    m_text.append(ba);
}

VfmdByteArray TextSpanTreeNode::text() const
{
    return m_text;
}

void TextSpanTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                  VfmdOutputDevice *outputDevice,
                                  VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        bool shouldUseSelfClosingTags = ((renderOptions & VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS) ==  VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS);
        int textRenderOptions = (HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES |
                                 HtmlTextRenderer::CONVERT_SPACE_SPACE_LF_TO_LINE_BREAK |
                                 HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                 HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE);
        if (shouldUseSelfClosingTags) {
            textRenderOptions = (textRenderOptions | HtmlTextRenderer::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS);
        }
        HtmlTextRenderer::render(m_text, outputDevice, textRenderOptions);
    }

    if (format == VfmdConstants::TREE_FORMAT) {
        renderTreePrefix(outputDevice, ancestorNodes, "+- span (text)\n");
        if ((renderOptions & VfmdConstants::TREE_RENDER_INCLUDES_TEXT) ==  VfmdConstants::TREE_RENDER_INCLUDES_TEXT) {
            renderTreeText(outputDevice, ancestorNodes, m_text);
        }
        if (hasChildren()) {
            renderTreePrefix(outputDevice, ancestorNodes, (hasNext()? "|  |\n" : "   |\n"));
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        }
    }
}
