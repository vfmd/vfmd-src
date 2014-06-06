#ifndef HTMLTEXTRENDERER_H
#define HTMLTEXTRENDERER_H

#include "vfmdbytearray.h"

class VfmdOutputDevice;

// HtmlTextRenderer:
// Renders HTML text to the output

class HtmlTextRenderer
{
public:
    static void renderURL(VfmdOutputDevice *outputDevice, const VfmdByteArray &text);
    static void renderCode(VfmdOutputDevice *outputDevice, const VfmdByteArray &text);
    static void renderTagAttribute(VfmdOutputDevice *outputDevice, const VfmdByteArray &text);
    static void renderURLAsText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text);
    static void renderText(VfmdOutputDevice *outputDevice, const VfmdByteArray &text,
                           bool isSelfClosingVoidTagsEnabled,
                           bool isLineBreakOnNewlinesEnabled);

private:
    HtmlTextRenderer(); // Cannot instantiate this class
};

#endif // HTMLTEXTRENDERER_H
