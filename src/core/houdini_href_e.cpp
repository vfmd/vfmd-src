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

/* From houdini_href_e.c */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "houdini.h"
#include "vfmdoutputdevice.h"
#include "core/vfmdcommonregexps.h"

/*
 * Escaping URLs in HTML documents:
 *
 * Per RFC 1738, anything except the following should be
 * percent-encoded:
 *    Alphanumerics:        [A-Za-z0-9]
 *    Special characters:   $-_.+!*'(),
 *    Reserved characters:  ;/?:@=&
 *
 * In addition, the following should be left intact:
 *    Percent-encoder:      %
 *    Fragment delimiter:   #
 *
 * Per the above, & and ' need not be percent-encoded
 * but should instead be HTML entity encoded as per
 * OWASP recommendations:
 *    & as "&amp;" (if not already part of a character entity)
 *    ' as "&#x27;"
 *
 * FIXME: If % is not part of a valid hex sequence, should
 * encode the % as "%25"
 */

static const char HREF_SAFE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,   // !#$%()*+,-./
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,   // [0-9]:;=?
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // @[A-O]
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,   // [P-Z]_
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // [a-o]
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,   // [p-z]
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void houdini_escape_href(VfmdOutputDevice *outputDevice, const char *src, unsigned int size)
{
    static const uint8_t hex_chars[] = "0123456789ABCDEF";
    static VfmdRegexp reCharRef = VfmdCommonRegexps::htmlCharacterReference();
    size_t  i = 0, org;

    while (i < size) {
        org = i;
        while (i < size && HREF_SAFE[src[i]] != 0)
            i++;

        if (likely(i > org)) {
            outputDevice->write(src + org, i - org);
        }

        if (i >= size)
            break;

        switch (src[i]) {
        case '&':
            if (reCharRef.locateInStringWithoutCapturing(src + i, size - i, 0) == 0) {
                // Part of a character reference. Leave it intact.
                // TODO: Check if it's really a valid named reference
                outputDevice->write('&');
            } else {
                // Not part of a character reference
                outputDevice->write("&amp;", 5);
            }
            break;
        case '\'':
            outputDevice->write("&#x27;", 6);
            break;
        default: /* percent-encode */
            outputDevice->write('%');
            outputDevice->write(hex_chars[(src[i] >> 4) & 0xF]);
            outputDevice->write(hex_chars[src[i] & 0xF]);
        }

        i++;
    }
}
