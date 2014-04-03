#ifndef VFMDLINE_H
#define VFMDLINE_H

#include "vfmdbytearray.h"
#include "vfmdregexp.h"

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

    unsigned int leadingSpacesCount() const;
    char firstNonSpace() const;

    int indexOf(const VfmdRegexp& re) const;
    bool matches(const VfmdRegexp& re) const;

    bool isHorizontalRuleLine() const;

private:
    /* Disable copying instances of this class */
    VfmdLine(const VfmdLine &);
    VfmdLine& operator=(const VfmdLine &);

    VfmdByteArray m_lineContent;

    // Precomputed line data
    void ensureLineDataComputed();
    bool m_isLineDataComputed;
    unsigned int m_leadingSpacesCount;
    unsigned int m_isBlankLine;

    // Checking if the line is a <hr/>
    void ensureIsHorizontalRuleComputed();
    bool m_isHorizontalRuleComputed, m_isHorizontalRule;
};

#endif // VFMDLINE_H
