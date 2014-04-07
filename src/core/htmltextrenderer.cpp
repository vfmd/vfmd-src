#include "htmltextrenderer.h"
#include "vfmdoutputdevice.h"
#include "core/vfmdcommonregexps.h"
#include "core/houdini.h"
#include <stdio.h>

void HtmlTextRenderer::render(const VfmdByteArray &text, VfmdOutputDevice *outputDevice, int options)
{
    const bool shouldUseSelfClosingTags = ((options & HtmlTextRenderer::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS) == HtmlTextRenderer::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS);
    const bool shouldRemoveBackslashes = ((options & HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES) == HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES);
    const bool shouldConvertLFtoBR = ((options & HtmlTextRenderer::CONVERT_LF_TO_LINE_BREAK) == HtmlTextRenderer::CONVERT_LF_TO_LINE_BREAK);
    const bool shouldConvertSpaceSpaceLFtoBR = ((options & HtmlTextRenderer::CONVERT_SPACE_SPACE_LF_TO_LINE_BREAK) == HtmlTextRenderer::CONVERT_SPACE_SPACE_LF_TO_LINE_BREAK);
    const bool shouldEscapeLTAndGT = ((options & HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT) == HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT);
    const bool shouldEscapeQuote = ((options & HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE) == HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE);
    const bool shouldEscapeAmpAlways = ((options & HtmlTextRenderer::HTML_ESCAPE_ALL_AMP) == HtmlTextRenderer::HTML_ESCAPE_ALL_AMP);
    const bool shouldEscapeAmpUnlessCharRef = ((options & HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE) == HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE);

    const char *data_ptr = text.data();
    size_t sz = text.size();

    if (data_ptr && sz) {
        unsigned int i = 0;
        const char *p;
        unsigned int plainTextStart = 0;
        for (p = data_ptr; i < sz; p++, i++) {
            const char c = *p;
            switch (c) {

            case '\\':
                if (shouldRemoveBackslashes && ((i + 1) < sz) && text.isEscapedAtPosition(i + 1)) {
                    VfmdUnicodeProperties::GeneralCategoryMajorClass unicodeMajorClass = text.majorClassOfUTF8CharStartingAt(i + 1);
                    if (unicodeMajorClass == VfmdUnicodeProperties::ucp_P || unicodeMajorClass == VfmdUnicodeProperties::ucp_S) {
                        // Backslash is followed by punctuation/symbol, so suppress the backslash
                        if (i > plainTextStart) {
                            outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                        }
                        plainTextStart = i + 1;
                    }
                }
                break;

            case '\n':
                if (shouldConvertSpaceSpaceLFtoBR &&
                    ((i >= 2) && (*(p-1) == ' ') && (*(p-2) == ' '))) {
                    if ((i - 2) > plainTextStart) {
                        outputDevice->write(text.data() + plainTextStart, i - 2 - plainTextStart);
                    }
                    plainTextStart = i + 1;
                    if (shouldUseSelfClosingTags) {
                        outputDevice->write("<br />\n", 7);
                    } else {
                        outputDevice->write("<br>\n", 5);
                    }
                    break;
                }
                if (shouldConvertLFtoBR) {
                    if (i > plainTextStart) {
                        outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                    }
                    plainTextStart = i + 1;
                    if (shouldUseSelfClosingTags) {
                        outputDevice->write("<br />\n", 7);
                    } else {
                        outputDevice->write("<br>\n", 5);
                    }
                }
                break;

            case '<':
            case '>':
                if (shouldEscapeLTAndGT) {
                    if (i > plainTextStart) {
                        outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                    }
                    plainTextStart = i + 1;
                    outputDevice->write(c == '<'? "&lt;" : "&gt;", 4);
                }
                break;

            case '&':
                if (shouldEscapeAmpAlways) {
                    if (i > plainTextStart) {
                        outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                    }
                    plainTextStart = i + 1;
                    outputDevice->write("&amp;", 5);
                } else if (shouldEscapeAmpUnlessCharRef) {
                    VfmdRegexp reCharRef = VfmdCommonRegexps::htmlCharacterReference();
                    if (reCharRef.locateInStringWithoutCapturing(p, sz - i, 0) < 0) {
                        // Not a character reference
                        // TODO: Check if it's really a valid named reference
                        if (i > plainTextStart) {
                            outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                        }
                        plainTextStart = i + 1;
                        outputDevice->write("&amp;", 5);
                    }
                }
                break;

            case '"':
                if (shouldEscapeQuote) {
                    if (i > plainTextStart) {
                        outputDevice->write(text.data() + plainTextStart, i - plainTextStart);
                    }
                    plainTextStart = i + 1;
                    outputDevice->write("&quot;", 6);
                }
                break;
            }
        }
        if (sz > plainTextStart) {
            outputDevice->write(text.data() + plainTextStart, sz - plainTextStart);
        }
    }
}

void HtmlTextRenderer::renderURL(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_href(outputDevice, text.data(), text.size());
}

void HtmlTextRenderer::renderCode(VfmdOutputDevice *outputDevice, const VfmdByteArray &text)
{
    houdini_escape_html(outputDevice, text.data(), text.size());
}
