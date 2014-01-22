#include <assert.h>
#include <stdio.h>
#include "pcre.h"
#include "vfmdregexp.h"

#define MAX_CAPTURE_COUNT 40

class VfmdRegexp::Private {
public:
    Private(const char *patternStr) : refCount(1) {
        // TODO: Optimize using pcre_study
        // TODO: Check that there are no backrefs using pcre_fullinfo
        const char *errorMsg = 0;
        int errorOffsetInString = 0;
        pcreRegexp = pcre_compile(patternStr,
                                  (PCRE_UTF8 | PCRE_NO_UTF8_CHECK),
                                  &errorMsg, &errorOffsetInString,
                                  0 /* use default character tables */);
        if (pcreRegexp) {
            pcre_refcount(pcreRegexp, 1);
        } else {
            printf("VfmdRegexp: Bad regexp pattern [%s]. Error \"%s\" at position %d.\n", patternStr, errorMsg, errorOffsetInString);
        }
        capturedTextCount = 0;
    }

    Private(pcre *p, const VfmdByteArray &s, int ctc, int *cd) : refCount(1) {
        pcreRegexp = p;
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
        }
    }

    unsigned int refCount;

    pcre *pcreRegexp;
    VfmdByteArray subjectBa;
    int capturedTextCount;
    int captureData[MAX_CAPTURE_COUNT * 3];
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
    if (!isValid()) {
        return -1;
    }

    copyOnWrite();
    d->subjectBa = ba;
    int matchCode = pcre_exec(d->pcreRegexp,
                              0 /*pcre_study data*/,
                              ba.data(), ba.size(), offset,
                              PCRE_NO_UTF8_CHECK,
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

    if (matchCode >= 0) {
        return d->captureData[0];
    }
    return -1;
}

bool VfmdRegexp::match(const VfmdByteArray &ba)
{
    return (indexIn(ba) >= 0);
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
        d = new Private(d->pcreRegexp, d->subjectBa, d->capturedTextCount, d->captureData);
    }
}
