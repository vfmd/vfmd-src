#ifndef VFMDLINE_H
#define VFMDLINE_H

#include "vfmdbytearray.h"

/* Conceptually represents a line in a vfmd document. */

class VfmdLine
{
public:
    /* Create a line encapsulating the given bytearray */
    VfmdLine(const VfmdByteArray &ba);

    /* Return the bytes encapsulated by the line */
    VfmdByteArray content() const;

    unsigned int size() const;
    char firstByte() const; // If the line has bytes, returns the first byte. Else returns 0.

    /* Is this line composed of only whitespace? */
    bool isBlankLine() const;

    VfmdLine *copy() const;

    unsigned int leadingSpacesCount() const;
    char firstNonSpace() const;
    void chopLeft(unsigned int n);
    void chopRight(unsigned int n);

private:
    /* Disable copying instances of this class */
    VfmdLine(const VfmdLine &);
    VfmdLine& operator=(const VfmdLine &);

    VfmdByteArray m_lineContent;
};

#endif // VFMDLINE_H
