#include "VfmdPreprocessor.h"
#include <stdlib.h>
#include <stdio.h>

VfmdPreprocessor::VfmdPreprocessor()
    : m_buffer(0)
    , m_bufferSize(4096)
    , m_isBufferAllocated(false)
    , m_filledBytes(0)
    , m_lineCallback(0)
    , m_lineCallbackContext(0)
{
}

VfmdPreprocessor::~VfmdPreprocessor()
{
    free(m_buffer);
}

bool VfmdPreprocessor::setBufferSize(unsigned int size)
{
    if (!m_isBufferAllocated && size >= 256) {
        m_bufferSize = size;
        return true;
    }
    return false;
}

unsigned int VfmdPreprocessor::bufferSize() const
{
    return m_bufferSize;
}

void VfmdPreprocessor::ensureBufferAllocated()
{
    if (!m_isBufferAllocated) {
        m_buffer = static_cast<char*>(malloc(m_bufferSize));
        m_isBufferAllocated = true;
    }
}

void VfmdPreprocessor::setLineCallback(LineCallbackFunc fn)
{
    m_lineCallback = fn;
}

void VfmdPreprocessor::setLineCallbackContext(void *context)
{
    m_lineCallbackContext = context;
}

/* The following code is adapted from pcre_valid_utf8.c
 * from the PCRE project (http://www.pcre.org/)
 */

/*
-----------------------------------------------------------------------------
                       Written by Philip Hazel
           Copyright (c) 1997-2013 University of Cambridge

                 Adapted for vfmd by Roopesh Chander
        Copyright (c) 2013 Roopesh Chander <roop@forwardbias.in>
-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

/* Table of the number of extra bytes, indexed by the first byte masked with
0x3f. The highest number for a valid UTF-8 first byte is in fact 0x3d. */

const unsigned char utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };

// int addBytes(data, length);
//
// Adds length bytes from data to the preprocessor.
// Returns the number of bytes of data consumed from data.
// If length is greater than the bufferSize, then
// the return value can be less than length. Otherwise,
// the return value is equal to length.
// It is recommended that length <= (bufferSize / 4)

// TODOs:
// - Handle CRLF
// - Expand tabs
// - Handle 1 code point spanning 2 addBytes calls
// - Interpret invalid UTF-8 bytes as IEC-8859-1
//   instead of filtering them out completely

#define BYTES_TO_READ (data + length - p)

int VfmdPreprocessor::addBytes(char *_data, int length)
{
    ensureBufferAllocated();

    unsigned char *buf = reinterpret_cast<unsigned char *>(m_buffer);
    unsigned char *data = reinterpret_cast<unsigned char *>(_data);

    register unsigned char *p = data;
    register unsigned char *dst = buf + m_filledBytes;

    while (BYTES_TO_READ > 0) {
        register unsigned char ab, c, d;

        // Ensure we have enough space for a code point
        // Max code point size is 4 bytes
        // So, check if we have atleast 4 bytes left in the buffer
        if ((dst - buf + 4) > m_bufferSize) {
            if (m_lineCallback) {
                (*m_lineCallback)(m_lineCallbackContext, (const char *) buf, dst - buf);
            }
            m_filledBytes = 0;
            return (p - data);
        }

        c = *p++; // First byte

        if (c < 128) {                      /* ASCII character */
                                            /* No continuation bytes */
            if (c == 0x0a) { // LF
                if (m_lineCallback) {
                    (*m_lineCallback)(m_lineCallbackContext, (const char *) buf, dst - buf);
                }
                m_filledBytes = 0;
                dst = buf;
                continue;
            }
            *dst++ = c;
            continue;
        }

        if (c < 0xc0) {                     /* Isolated UTF-8 continuation byte */
                                            /* without a leading byte */
            continue;
        }

        if (c >= 0xfe) {                    /* Bytes 0xfe and 0xff */
                                            /* are invalid in UTF-8 */
            continue;
        }

        // ab is the number of additional bytes
        // or continuation bytes (1 <= ab <= 5)
        ab = utf8_table4[c & 0x3f];

        register unsigned char *p1 = p;
        if (BYTES_TO_READ >= ab) {
            if (ab < 4) {
                // Copy bytes to output without consuming the input
                *dst++ = c;
                for (int i = 0; i < ab; i++) {
                    *dst++ = *(p1 + i);
                }
            } else {
                // 5- and 6-byte sequences are considered unsafe
            }
        }

        d = *p++; // Second byte

        if ((ab < 4) && (d & 0xc0) != 0x80) {   /* Second byte not of the form 10xx xxxx */
            dst -= (ab + 1); // rewind the dst pointer to filter out the current code point
            continue;
        }

        /* For each continuation byte, check that the bytes start with the 0x80 bit
        set and not the 0x40 bit. Then check for an overlong sequence, and for the
        excluded range 0xd800 to 0xdfff. */

        switch (ab) {

            /* 2-byte character. No further bytes to check for 0x80. Check first byte
            for for xx00 000x (overlong sequence). */

            case 1:
                if ((c & 0x3e) == 0) {
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                break;

            /* 3-byte character. Check third byte for 0x80. Then check first 2 bytes
            for 1110 0000, xx0x xxxx (overlong sequence) or
                1110 1101, 1010 xxxx (0xd800 - 0xdfff) */

            case 2:
                if (((*p++) & 0xc0) != 0x80) { /* Third byte not of the form 10xx xxxx */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                if (c == 0xe0 && (d & 0x20) == 0) { /* Overlong sequence */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                if (c == 0xed && d >= 0xa0) { /* Code points between U+D800 and U+DFFF */
                                              /* are prohibited in UTF-8 */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                break;

            /* 4-byte character. Check 3rd and 4th bytes for 0x80. Then check first 2
            bytes for for 1111 0000, xx00 xxxx (overlong sequence), then check for a
            character greater than 0x0010ffff (f4 8f bf bf) */

            case 3:
                if (((*p++) & 0xc0) != 0x80) { /* Third byte not of the form 10xx xxxx */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Fourth byte not of the form 10xx xxxx */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                if (c == 0xf0 && (d & 0x30) == 0) { /* Overlong sequence */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                if (c > 0xf4 || (c == 0xf4 && d > 0x8f)) { /* Code points above 0x0010ffff */
                                                           /* are invalid */
                    dst -= (ab + 1); // filter out the current code point
                    continue;
                }
                break;

            /* 5-byte and 6-byte characters are not allowed by RFC 3629, and have
            already been rejected. However, we should see how many bytes should be
            consumed from the input data. If there are no errors, all continuing
            bytes are consumed. Else, we consume till we hit an errant byte. */

            /* 5-byte character. Check 3rd, 4th, and 5th bytes for 0x80. Then check for
            1111 1000, xx00 0xxx */

            case 4:
                if (((*p++) & 0xc0) != 0x80) { /* Third byte not of the form 10xx xxxx */
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Fourth byte not of the form 10xx xxxx */
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Fifth byte not of the form 10xx xxxx */
                    continue;
                }
                if (c == 0xf8 && (d & 0x38) == 0) { /* Overlong sequence */
                    continue;
                }
                break;

            /* 6-byte character. Check 3rd-6th bytes for 0x80. Then check for
            1111 1100, xx00 00xx. */

            case 5:
                if (((*p++) & 0xc0) != 0x80) { /* Third byte not of the form 10xx xxxx */
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Fourth byte not of the form 10xx xxxx */
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Fifth byte not of the form 10xx xxxx */
                    continue;
                }
                if (((*p++) & 0xc0) != 0x80) { /* Sixth byte not of the form 10xx xxxx */
                    continue;
                }
                if (c == 0xfc && (d & 0x3c) == 0) { /* Overlong sequence */
                    continue;
                }
                break;
        } // End of switch (ab)

    } // End of while (BYTES_TO_READ > 0)

    m_filledBytes = (dst - buf);
    return (p - data);
}
