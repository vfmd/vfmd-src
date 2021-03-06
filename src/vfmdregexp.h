#ifndef VFMDREGEXP_H
#define VFMDREGEXP_H

#include "vfmdbytearray.h"

class VfmdRegexp
{
public:
    VfmdRegexp(const char *pattern);
    ~VfmdRegexp();

    bool isValid() const;

    int indexIn(const VfmdByteArray &ba, int offset = 0);
    bool matches(const VfmdByteArray &ba);

    int lengthOfMatch() const;
    int captureCount() const; // Max 40 captures
    VfmdByteArray capturedText(int index = 0) const;

    int locateInStringWithoutCapturing(const char *str, int length = 0, int offset = 0) const;
    int locateInBytearrayWithoutCapturing(const VfmdByteArray &ba, int offset = 0) const;

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
