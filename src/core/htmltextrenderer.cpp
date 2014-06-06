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
    houdini_escape_htmlish(outputDevice, text, (REMOVE_ESCAPING_BACKSLASHES));
}

void HtmlTextRenderer::renderURLAsText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_htmlish(outputDevice, text);
}

void HtmlTextRenderer::renderText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text,
                                  bool isSelfClosingVoidTagsEnabled,
                                  bool isLineBreakOnNewlinesEnabled)
{
    houdini_escape_htmlish(outputDevice, text,
                           (REMOVE_ESCAPING_BACKSLASHES | INSERT_BR_TAGS),
                           isSelfClosingVoidTagsEnabled,
                           isLineBreakOnNewlinesEnabled);
}
