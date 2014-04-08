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

#ifndef __HOUDINI_H__
#define __HOUDINI_H__

#include "vfmdbytearray.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

class VfmdOutputDevice;

enum HtmlishTextProcessingOptions {
    REMOVE_ESCAPING_BACKSLASHES = 1,
    INSERT_BR_TAGS = 2
};

// Input is html-ish text (for e.g. input can have HTML entities like "&copy;").
// Output should be a URL that can be included in HTML.
extern void houdini_escape_href(VfmdOutputDevice *outputDevice, const char *src, unsigned int size);

// Input is definitely text. Output should be HTML.
extern void houdini_escape_html(VfmdOutputDevice *outputDevice, const char *src, unsigned int size);

// Input is html-ish text (for e.g. input can have HTML entities like "&copy;").
// Escaping backslashes are always removed.
// 'textProcessingOptions' can be combinations of HtmlishTextProcessingOptions.
// 'renderOptions' can be combinations of VfmdConstants::HtmlRenderOptions.
// Output should be HTML.
extern void houdini_escape_htmlish(VfmdOutputDevice *outputDevice, const VfmdByteArray &text,
                                   int textProcessingOptions, int renderOptions);

#endif // __HOUDINI_H__
