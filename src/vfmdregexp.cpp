#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pcre.h"
#include "vfmdregexp.h"
#include "vfmdscopedpointer.h"

#define MAX_CAPTURE_COUNT 40

class VfmdRegexp::Private {
public:
    Private(const char *patternStr) : refCount(1) {
        // TODO: Check that there are no backrefs using pcre_fullinfo
        const char *errorMsg = 0;
        int errorOffsetInString = 0;
        int options = PCRE_UTF8;
#ifndef VFMD_DEBUG
        options = (options | PCRE_NO_UTF8_CHECK);
#endif
        pcreRegexp = pcre_compile(patternStr,
                                  options,
                                  &errorMsg, &errorOffsetInString,
                                  0 /* use default character tables */);
        if (pcreRegexp) {
            pcreExtra = pcre_study(pcreRegexp, 0, &errorMsg);
            pcre_refcount(pcreRegexp, 1);
        } else {
            printf("VfmdRegexp: Bad regexp pattern [%s]. Error \"%s\" at position %d.\n", patternStr, errorMsg, errorOffsetInString);
        }
        capturedTextCount = 0;
        matchFound = false;
    }

    Private(pcre *p, pcre_extra *e, const VfmdByteArray &s, int ctc, int *cd) : refCount(1) {
        pcreRegexp = p;
        pcreExtra = e;
        subjectBa = s;
        for (int i = 0; i < ctc; i++) {
            captureData[i] = cd[i];
        }
        capturedTextCount = ctc;
        if (pcreRegexp) {
            pcre_refcount(pcreRegexp, 1);
        }
    }

    ~Private() {
        if (pcre_refcount(pcreRegexp, -1) == 0) {
            free(pcreRegexp);
            if (pcreExtra) {
                pcre_free_study(pcreExtra);
            }
        }
    }

    unsigned int refCount;

    pcre *pcreRegexp;
    pcre_extra *pcreExtra;
    VfmdByteArray subjectBa;
    int capturedTextCount;
    int captureData[MAX_CAPTURE_COUNT * 3];
    bool matchFound;
};

VfmdRegexp::VfmdRegexp(const char *pattern)
    : d(new Private(pattern))
{
}

VfmdRegexp::~VfmdRegexp()
{
    deref();
}

bool VfmdRegexp::isValid() const
{
    return (d->pcreRegexp != 0);
}

int VfmdRegexp::indexIn(const VfmdByteArray &ba, int offset)
{
    if (!isValid() || !ba.isValid()) {
        return -1;
    }

    if (ba == d->subjectBa) {
        // Nothing to do
        if (d->matchFound) {
            return d->captureData[0];
        }
        return -1;
    }

    copyOnWrite();
    int options = 0;
#ifndef VFMD_DEBUG
        options = (options | PCRE_NO_UTF8_CHECK);
#endif
    int matchCode = pcre_exec(d->pcreRegexp,
                              d->pcreExtra,
                              ba.data(), ba.size(), offset,
                              options,
                              d->captureData, (MAX_CAPTURE_COUNT * 3));
    int capturedTextCount = 0;
    if (matchCode > 0) {
        // We've got a match
        capturedTextCount = matchCode;
    } else if (matchCode == 0) {
        // PCRE ran out of space in the output vector
        capturedTextCount = MAX_CAPTURE_COUNT;
    } else if (matchCode != PCRE_ERROR_NOMATCH) {
        // Some error
        printf("VfmdRegexp: Matching failed with PCRE error code: %d.\n", matchCode);
    }
    d->capturedTextCount = capturedTextCount;
    d->subjectBa = ba;

    if (matchCode >= 0) {
        d->matchFound = true;
        return d->captureData[0];
    }
    d->matchFound = false;
    return -1;
}

int VfmdRegexp::locateInStringWithoutCapturing(const char *str, int length, int offset) const
{
    if (!isValid() || str == 0) {
        return -1;
    }

    if (length == 0) {
        length = strlen(str);
    }

    int options = 0;
#ifndef VFMD_DEBUG
        options = (options | PCRE_NO_UTF8_CHECK);
#endif
    int captureData[3];
    int matchCode = pcre_exec(d->pcreRegexp,
                              d->pcreExtra,
                              str, length, offset,
                              options,
                              captureData, 3 /* Capture only the entire-match range */);
    if (matchCode >= 0) {
        // We've got a match
        return captureData[0];
    }

    if (matchCode != PCRE_ERROR_NOMATCH) {
        // Some error
        printf("VfmdRegexp: Matching failed with PCRE error code: %d.\n", matchCode);
    }

    // No match
    return -1;
}

int VfmdRegexp::locateInBytearrayWithoutCapturing(const VfmdByteArray &ba, int offset) const
{
    if (!isValid() || !ba.isValid()) {
        return -1;
    }
    return locateInStringWithoutCapturing(ba.data(), ba.size(), offset);
}

bool VfmdRegexp::matches(const VfmdByteArray &ba)
{
    return (indexIn(ba) >= 0);
}

int VfmdRegexp::lengthOfMatch() const
{
    if (d->capturedTextCount > 0) {
        return (d->captureData[1] - d->captureData[0]);
    }
    return 0;
}

int VfmdRegexp::captureCount() const
{
    return d->capturedTextCount;
}

VfmdByteArray VfmdRegexp::capturedText(int index) const
{
    if ((index >= 0) && (index < captureCount())) {
        int fromPos = d->captureData[index * 2];
        int toPos = d->captureData[(index * 2) + 1];
        return d->subjectBa.mid(fromPos, toPos - fromPos);
    }
    return VfmdByteArray();
}

// Implicit sharing stuff follows

VfmdRegexp::VfmdRegexp(const VfmdRegexp &other)
    : d(other.d) {
    ref();
}

VfmdRegexp& VfmdRegexp::operator=(const VfmdRegexp &other) {
    if (this != &other) {
        deref(); // dereference existing data
        d = other.d;
        ref();   // reference new data
    }
    return *this;
}

void VfmdRegexp::ref() {
    assert(d);
    assert(d->refCount);
    d->refCount++;
}

void VfmdRegexp::deref() {
    assert(d);
    assert(d->refCount);
    d->refCount--;
    if (d->refCount == 0) {
        delete d;
        d = 0;
    }
}

void VfmdRegexp::copyOnWrite() {
    if (d->refCount > 1) {
        deref(); // dereference existing data
        // copy data (refCount will be 1 for new copy)
        d = new Private(d->pcreRegexp, d->pcreExtra, d->subjectBa, d->capturedTextCount, d->captureData);
    }
}
