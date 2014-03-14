/* The following code is adapted from the PCRE project
 * http://www.pcre.org/
 */

/*
-----------------------------------------------------------------------------
                       Written by Philip Hazel
           Copyright (c) 1997-2013 University of Cambridge
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

#ifndef SCANLINE_C
#define SCANLINE_C

#ifdef __cplusplus
extern "C" {
#endif

int scan_line(const unsigned char* string, int length, int *erroroffset, int *numOfCodePoints);

#ifdef __cplusplus
}
#endif

#define SCANLINE_EOL                     0
#define SCANLINE_UTF8_ERR1               1
#define SCANLINE_UTF8_ERR2               2
#define SCANLINE_UTF8_ERR3               3
#define SCANLINE_UTF8_ERR4               4
#define SCANLINE_UTF8_ERR5               5
#define SCANLINE_UTF8_ERR6               6
#define SCANLINE_UTF8_ERR7               7
#define SCANLINE_UTF8_ERR8               8
#define SCANLINE_UTF8_ERR9               9
#define SCANLINE_UTF8_ERR10             10
#define SCANLINE_UTF8_ERR11             11
#define SCANLINE_UTF8_ERR12             12
#define SCANLINE_UTF8_ERR13             13
#define SCANLINE_UTF8_ERR14             14
#define SCANLINE_UTF8_ERR15             15
#define SCANLINE_UTF8_ERR16             16
#define SCANLINE_UTF8_ERR17             17
#define SCANLINE_UTF8_ERR18             18
#define SCANLINE_UTF8_ERR19             19
#define SCANLINE_UTF8_ERR20             20
#define SCANLINE_UTF8_ERR21             21
#define SCANLINE_UTF8_ERR22             22  /* Unused (was non-character) */

#define SCANLINE_TAB_BYTE               64

#endif // SCANLINE_C
