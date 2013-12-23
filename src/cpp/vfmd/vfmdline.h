#ifndef VFMDLINE_H
#define VFMDLINE_H

#include "vfmdbytearray.h"

/* Conceptually represents a line in a vfmd document.
 * Please don't add data with CR / CRLF to the line
 * (there are no checks to prevent that) */

class VfmdLine : public VfmdByteArray
{
public:
    /* Create an empty and invalid line */
    VfmdLine();

    /* Create a line with a copy of 'length' bytes starting from 'data' */
    VfmdLine(const char *data, int length);

    /* Is this line composed of only whitespace? */
    bool isBlankLine() const;
};

#endif // VFMDLINE_H
