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

#define BYTES_TO_READ (data + length - p)
#define CONVERT_FROM_ISO_8859_1(ptr, c) \
        if (c < 128) { \
            *ptr++ = (c); \
        } else { \
            *ptr++ = (0xc0 | (((c) >> 6) & 0x03)); \
            *ptr++ = (0x80 | ((c) & 0x3f)); \
        }

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
        // But if each byte of a 4-byte invalid UTF-8 sequence is
        // interpreted as ISO-8859-1, we could end up with 8 bytes.
        // So, check if we have atleast 8 bytes left in the buffer.
        if ((dst - buf + 8) > m_bufferSize) {
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
            CONVERT_FROM_ISO_8859_1(dst, c);
            continue;
        }

        if (c >= 0xfe) {                    /* Bytes 0xfe and 0xff */
                                            /* are invalid in UTF-8 */
            CONVERT_FROM_ISO_8859_1(dst, c);
            continue;
        }

        // ab is the number of additional bytes
        // or continuation bytes (1 <= ab <= 5)
        ab = utf8_table4[c & 0x3f];

        // 5-byte and 6-byte sequences are not valid UTF-8 as per RFC 3629

        if (ab > 3) {
            // Assume first byte to be in ISO-8859-1 encoding.
            // Rest of the bytes are not consumed.
            CONVERT_FROM_ISO_8859_1(dst, c);
            continue;
        }

        if (BYTES_TO_READ == 0) {
            // TODO: Remember leading byte
            continue;
        }

        // Overlong sequences and invalid code points

        switch (ab) {

            /* 2-byte character.
             * Check first byte for xx00 000x (overlong sequence) */

            case 1:
                if ((c & 0x3e) == 0) {                  /* Overlong sequence */
                    // Assume first byte to be in ISO-8859-1 encoding
                    // First continuation byte is not consumed
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    continue;
                }
                d = *p++; // Second byte
                break;

            /* 3-byte character.
             * Check first 2 bytes for 1110 0000, xx0x xxxx (overlong sequence)
             * Check first 2 bytes for 1110 1101, 1010 xxxx (0xd800 - 0xdfff) */

            case 2:
                d = *p++; // Second byte
                if (
                    (c == 0xe0 && (d & 0x20) == 0) ||   /* Overlong sequence */
                    (c == 0xed && d >= 0xa0)    /* Code points between U+D800 and U+DFFF */
                                                /* are prohibited in UTF-8 */

                   ) {
                    // Assume first 2 bytes to be in ISO-8859-1 encoding.
                    // Third byte is not consumed.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    continue;
                }
                break;

            /* 4-byte character.
             * Check first 2 bytes for for 1111 0000, xx00 xxxx (overlong sequence)
             * Check for code point greater than 0x0010ffff (f4 8f bf bf) */

            case 3:
                d = *p++; // Second byte
                if (
                    (c == 0xf0 && (d & 0x30) == 0) ||       /* Overlong sequence */
                    (c > 0xf4 || (c == 0xf4 && d > 0x8f))   /* Code points above 0x0010ffff */
                                                            /* are invalid */
                   ) {
                    // Assume first 2 bytes to be in ISO-8859-1 encoding.
                    // Last 2 bytes are not consumed.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    continue;
                }
                break;
        }

        // Each trailing byte should be of the form 10xx xxxx

        if (BYTES_TO_READ < (ab - 1)) {
            // TODO: Remember trailing byte
            continue;
        }

        unsigned char e, f;
        switch (ab) {

            case 1:
                if ((d & 0xc0) != 0x80) {   /* Second byte not of the form 10xx xxxx */
                    // Assume both bytes to be in ISO-8859-1 encoding.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    continue;
                }
                break;

            case 2:
                if ((d & 0xc0) != 0x80) {   /* Second byte not of the form 10xx xxxx */
                    // Assume first 2 bytes to be in ISO-8859-1 encoding.
                    // Third byte is not consumed.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    continue;
                }
                e = *p++; // Third byte
                if ((e & 0xc0) != 0x80) {   /* Third byte not of the form 10xx xxxx */
                    // Assume all 3 bytes to be in ISO-8859-1 encoding.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    CONVERT_FROM_ISO_8859_1(dst, e);
                    continue;
                }
                break;

            case 3:
                if ((d & 0xc0) != 0x80) {   /* Second byte not of the form 10xx xxxx */
                    // Assume first 2 bytes to be in ISO-8859-1 encoding.
                    // Last 2 bytes are not consumed.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    continue;
                }
                e = *p++; // Third byte
                if ((e & 0xc0) != 0x80) {   /* Third byte not of the form 10xx xxxx */
                    // Assume first 3 bytes to be in ISO-8859-1 encoding.
                    // Fourth byte is not consumed.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    CONVERT_FROM_ISO_8859_1(dst, e);
                    continue;
                }
                f = *p++; // Fourth byte
                if ((e & 0xc0) != 0x80) {   /* Fourth byte not of the form 10xx xxxx */
                    // Assume all 4 bytes to be in ISO-8859-1 encoding.
                    CONVERT_FROM_ISO_8859_1(dst, c);
                    CONVERT_FROM_ISO_8859_1(dst, d);
                    CONVERT_FROM_ISO_8859_1(dst, e);
                    CONVERT_FROM_ISO_8859_1(dst, f);
                    continue;
                }
                break;

        }

        // If we're here, this is a valid UTF-8 code point.
        // Just copy it.

        switch (ab) {
            case 0:
                *dst++ = c;
                break;
            case 1:
                *dst++ = c;
                *dst++ = d;
                break;
            case 2:
                *dst++ = c;
                *dst++ = d;
                *dst++ = e;
                break;
            case 3:
                *dst++ = c;
                *dst++ = d;
                *dst++ = e;
                *dst++ = f;
                break;
        }

    } // End of while (BYTES_TO_READ > 0)

    m_filledBytes = (dst - buf);
    return (p - data);
}
