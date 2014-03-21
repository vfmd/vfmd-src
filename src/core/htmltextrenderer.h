#ifndef HTMLTEXTRENDERER_H
#define HTMLTEXTRENDERER_H

#include "vfmdbytearray.h"

class VfmdOutputDevice;

// HtmlTextRenderer:
// Renders HTML text to the output
// Always removes escaping backslashes. Other functionalities are configurable as options.

class HtmlTextRenderer
{
public:
    enum Options {
        DEFAULT_OPTIONS = 0,
        HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS = 1,
        REMOVE_ESCAPING_BACKSLASHES = 2,
        CONVERT_LF_TO_LINE_BREAK = 4,
        CONVERT_SPACE_SPACE_LF_TO_LINE_BREAK = 8,
        HTML_ESCAPE_ALL_LT_GT = 16,
        HTML_ESCAPE_ALL_QUOTE = 32,
        HTML_ESCAPE_ALL_AMP = 64,
        HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE = 128
    };

    static void render(const VfmdByteArray &text, VfmdOutputDevice *outputDevice, int options = HtmlTextRenderer::DEFAULT_OPTIONS);

private:
    HtmlTextRenderer(); // Cannot instantiate this class
};

#endif // HTMLTEXTRENDERER_H
