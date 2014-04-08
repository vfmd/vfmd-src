#include "htmltextrenderer.h"
#include "vfmdoutputdevice.h"
#include "core/vfmdcommonregexps.h"
#include "core/houdini.h"
#include <stdio.h>

void HtmlTextRenderer::renderURL(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_href(outputDevice, text.data(), text.size());
}

void HtmlTextRenderer::renderCode(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_html(outputDevice, text.data(), text.size());
}

void HtmlTextRenderer::renderTagAttribute(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_htmlish(outputDevice, text, (REMOVE_ESCAPING_BACKSLASHES), 0);
}

void HtmlTextRenderer::renderURLAsText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_htmlish(outputDevice, text, 0, 0);
}

void HtmlTextRenderer::renderText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text, int htmlRenderOptions)
{
    houdini_escape_htmlish(outputDevice, text, (REMOVE_ESCAPING_BACKSLASHES | INSERT_BR_TAGS), htmlRenderOptions);
}
