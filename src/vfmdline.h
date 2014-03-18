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

    void chopLeft(unsigned int n);
    void chopRight(unsigned int n);

    void precomputeLineData();
    unsigned int leadingSpacesCount() const;
    char firstNonSpace() const;

private:
    /* Disable copying instances of this class */
    VfmdLine(const VfmdLine &);
    VfmdLine& operator=(const VfmdLine &);

    VfmdByteArray m_lineContent;

    // Precomputed line data
    bool m_isLineDataPrecomputed;
    unsigned int m_leadingSpacesCount;
    unsigned int m_isBlankLine;
};

#endif // VFMDLINE_H
