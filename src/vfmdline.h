#ifndef VFMDLINE_H
#define VFMDLINE_H

#include "vfmdbytearray.h"

/* Conceptually represents a line in a vfmd document.
 * All lines must end with an LF character */

class VfmdLine : public VfmdByteArray
{
public:
    /* Create an empty and invalid line */
    VfmdLine();

    /* Implicit cast from a bytearray, without checking for ending newline */
    VfmdLine(const VfmdByteArray &ba);

    /* Create a line with a copy of the null-terminated string 'str' */
    VfmdLine(const char *str);

    /* Create a line with a copy of 'length' bytes starting from 'data' */
    VfmdLine(const char *data, int length);

    /* Is this line composed of only whitespace? */
    bool isBlankLine() const;
};

#endif // VFMDLINE_H
