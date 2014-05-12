/* The following code is adapted from Houdini
 * https://github.com/vmg/houdini/
 */

/*
 * Copyright (C) 2012 Vicent Martí
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* From houdini_html_e.c */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "houdini.h"
#include "vfmdoutputdevice.h"
#include "vfmdconstants.h"
#include "core/vfmdcommonregexps.h"

/**
 * According to the OWASP rules:
 *
 * & --> &amp;
 * < --> &lt;
 * > --> &gt;
 * " --> &quot;
 * ' --> &#x27;
 * [forward-slash (/) is not escaped]
 */

static const char HTML_ESCAPE_TABLE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,    // "&'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0,    // <>
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char *HTML_ESCAPES[] = {
        "",
        "&quot;", // "
        "&amp;",  // &
        "&#x27;", // '
        "&lt;",   // <
        "&gt;"    // >
};

void houdini_escape_html(VfmdOutputDevice *outputDevice, const char *src, unsigned int size)
{
    size_t i = 0, org, esc = 0;

    while (i < size) {
        org = i;
        while (i < size && (esc = HTML_ESCAPE_TABLE[src[i]]) == 0)
            i++;

        if (likely(i > org)) {
            outputDevice->write(src + org, i - org);
        }

        if (i >= size)
            break;

        outputDevice->write(HTML_ESCAPES[esc]);

        i++;
    }
}

static const char HTMLISH_ESCAPE_TABLE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0,    // LF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,    // "&'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0,    // <>
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0,    // Backslash
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void houdini_escape_htmlish(VfmdOutputDevice *outputDevice, const VfmdByteArray &text,
                            int textProcessingOptions,
                            bool isSelfClosingVoidTagsEnabled,
                            bool isLineBreakOnNewlinesEnabled)
{
    const char *src = text.data();
    const unsigned char *src_u = reinterpret_cast<const unsigned char *>(src);
    size_t size = text.size();
    size_t i = 0, org, esc = 0;
    static VfmdRegexp reCharRef = VfmdCommonRegexps::htmlCharacterReference();

    bool shouldConvertSpaceSpaceLFtoBR = false;
    bool shouldConvertLFtoBR = false;
    if ((textProcessingOptions & INSERT_BR_TAGS) == INSERT_BR_TAGS) {
        shouldConvertSpaceSpaceLFtoBR = true;
        shouldConvertLFtoBR = isLineBreakOnNewlinesEnabled;
    }
    bool shouldRemoveEscBackslashes = ((textProcessingOptions & REMOVE_ESCAPING_BACKSLASHES) == REMOVE_ESCAPING_BACKSLASHES);
    bool shouldUseSelfClosingTags = isSelfClosingVoidTagsEnabled;

    while (i < size) {
        org = i;
        while (i < size && (esc = HTMLISH_ESCAPE_TABLE[src_u[i]]) == 0)
            i++;

        if (i < size) {

            assert(esc != 0);

            switch(esc) {
            case 6: // Backslash
                if (likely(i > org)) {
                    outputDevice->write(src + org, i - org);
                }
                if (shouldRemoveEscBackslashes && (i + 1) < size) {
                    VfmdUnicodeProperties::GeneralCategoryMajorClass unicodeMajorClass = text.majorClassOfUTF8CharStartingAt(i + 1);
                    if (unicodeMajorClass != VfmdUnicodeProperties::ucp_P && unicodeMajorClass != VfmdUnicodeProperties::ucp_S) {
                        // Backslash is not followed by punctuation/symbol, so output the backslash
                        outputDevice->write('\\');
                    } else if (src[i + 1] == '\\') {
                        // If the following byte is a backslash, output that too (to avoid it being suppressed later)
                        outputDevice->write('\\');
                        i++;
                    }
                } else {
                    outputDevice->write('\\');
                }
                break;

            case 7: // LF
                if (shouldConvertSpaceSpaceLFtoBR &&
                    ((i >= 2) && (src[i - 1] == ' ') && (src[i - 2] == ' '))) {
                    if (likely((i - 2) > org)) {
                        outputDevice->write(src + org, i - 2 - org);
                    }
                    if (shouldUseSelfClosingTags) {
                        outputDevice->write("<br />\n", 7);
                    } else {
                        outputDevice->write("<br>\n", 5);
                    }
                } else {
                    if (likely(i > org)) {
                        outputDevice->write(src + org, i - org);
                    }
                    if (shouldConvertLFtoBR) {
                        if (shouldUseSelfClosingTags) {
                            outputDevice->write("<br />\n", 7);
                        } else {
                            outputDevice->write("<br>\n", 5);
                        }
                    } else {
                        outputDevice->write('\n');
                    }
                }
                break;

            case 2: // Ampersand
                if (likely(i > org)) {
                    outputDevice->write(src + org, i - org);
                }
                if (reCharRef.locateInStringWithoutCapturing(src + i, size - i, 0) == 0) {
                    // Part of a character reference. Leave it intact.
                    // TODO: Check if it's really a valid named reference
                    outputDevice->write('&');
                } else {
                    // Not part of a character reference
                    outputDevice->write("&amp;", 5);
                }
                break;

            default:
                if (likely(i > org)) {
                    outputDevice->write(src + org, i - org);
                }
                outputDevice->write(HTML_ESCAPES[esc]);
            }

            i++;
            continue;
        }

        assert(i == size);
        if (likely(i > org)) {
            outputDevice->write(src + org, i - org);
        }
    }
}
