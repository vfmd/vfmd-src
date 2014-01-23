#ifndef VFMDREGEXP_H
#define VFMDREGEXP_H

#include "vfmdbytearray.h"

class VfmdLineArrayIterator;

class VfmdRegexp
{
public:
    VfmdRegexp(const char *pattern);
    ~VfmdRegexp();

    bool isValid() const;

    int indexIn(const VfmdByteArray &ba, int offset = 0);
    bool match(const VfmdByteArray &ba);

    int captureCount() const; // Max 40 captures
    VfmdByteArray capturedText(int index = 0) const;

    /* moveIteratorForward():
       If this regexp matches the text that the
       iterator points to, move the iterator over the matching
       text. The match must start at the current position
       of the iterator. No subpattern matches are captured. */
    bool moveIteratorForward(VfmdLineArrayIterator *iterator) const;

    // Copying regexps
    VfmdRegexp(const VfmdRegexp &other);
    VfmdRegexp& operator=(const VfmdRegexp& other);

private:
    void ref();
    void deref();
    void copyOnWrite();

    class Private;
    Private *d;
};

#endif // VFMDREGEXP_H
