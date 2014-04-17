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

void TextSpanTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                  VfmdOutputDevice *outputDevice,
                                  VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        HtmlTextRenderer::renderText(outputDevice, m_text, renderOptions);
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
