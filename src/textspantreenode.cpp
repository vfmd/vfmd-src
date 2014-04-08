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
        HtmlTextRenderer::renderText(outputDevice, m_text, renderOptions);
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
