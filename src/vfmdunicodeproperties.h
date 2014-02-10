#ifndef VFMDUNICODEPROPERTIES_H
#define VFMDUNICODEPROPERTIES_H

/* The following code is adapted from ucp.h in
 * the PCRE project (http://www.pcre.org/)
 */

/*
-----------------------------------------------------------------------------
                       Written by Philip Hazel
           Copyright (c) 1997-2013 University of Cambridge

            Adapted with minor changes by Roopesh Chander
                 Copyright (c) 2014 Roopesh Chander
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

// VfmdUnicodeProperties:
// This class contains constants that describe
// Unicode Character Properties for Unicode Code Points

class VfmdUnicodeProperties {
public:

    enum GeneralCategoryMajorClass {
        ucp_UndefinedGeneralCategoryMajorClass = -1,
        ucp_C,     /* Other */
        ucp_L,     /* Letter */
        ucp_M,     /* Mark */
        ucp_N,     /* Number */
        ucp_P,     /* Punctuation */
        ucp_S,     /* Symbol */
        ucp_Z      /* Separator */
    };

    enum GeneralCategory {
        ucp_UndefinedGeneralCategory = -1,
        ucp_Cc,    /* Control */
        ucp_Cf,    /* Format */
        ucp_Cn,    /* Unassigned */
        ucp_Co,    /* Private use */
        ucp_Cs,    /* Surrogate */
        ucp_Ll,    /* Lower case letter */
        ucp_Lm,    /* Modifier letter */
        ucp_Lo,    /* Other letter */
        ucp_Lt,    /* Title case letter */
        ucp_Lu,    /* Upper case letter */
        ucp_Mc,    /* Spacing mark */
        ucp_Me,    /* Enclosing mark */
        ucp_Mn,    /* Non-spacing mark */
        ucp_Nd,    /* Decimal number */
        ucp_Nl,    /* Letter number */
        ucp_No,    /* Other number */
        ucp_Pc,    /* Connector punctuation */
        ucp_Pd,    /* Dash punctuation */
        ucp_Pe,    /* Close punctuation */
        ucp_Pf,    /* Final punctuation */
        ucp_Pi,    /* Initial punctuation */
        ucp_Po,    /* Other punctuation */
        ucp_Ps,    /* Open punctuation */
        ucp_Sc,    /* Currency symbol */
        ucp_Sk,    /* Modifier symbol */
        ucp_Sm,    /* Mathematical symbol */
        ucp_So,    /* Other symbol */
        ucp_Zl,    /* Line separator */
        ucp_Zp,    /* Paragraph separator */
        ucp_Zs     /* Space separator */
    };

};

#endif // VFMDUNICODEPROPERTIES_H
