#include <assert.h>
#include <stdio.h>
#include "pcre.h"
#include "vfmdregexp.h"
#include "vfmdlinearrayiterator.h"
#include "vfmdscopedpointer.h"

#define MAX_CAPTURE_COUNT 40

class VfmdRegexp::Private {
public:
    Private(const char *patternStr) : refCount(1) {
        // TODO: Optimize using pcre_study
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
    int options = 0;
#ifndef VFMD_DEBUG
        options = (options | PCRE_NO_UTF8_CHECK);
#endif
    int matchCode = pcre_exec(d->pcreRegexp,
                              0 /*pcre_study data*/,
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

bool VfmdRegexp::moveIteratorForward(VfmdLineArrayIterator *iterator) const
{
    if (!isValid()) {
        return false;
    }

    VfmdLineArrayIterator iter = (*iterator);
    int options = (PCRE_ANCHORED | PCRE_NOTEMPTY);
#ifndef VFMD_DEBUG
        options = (options | PCRE_NO_UTF8_CHECK);
#endif
    int captureData[2];
    int workspace[64];
    while (!iter.isAtEnd()) {
        VfmdByteArray fragment = iter.bytesTillEndOfLine();
        int optionsForDfaMatch = options;
        if (!iter.isAtLastLine()) {
            optionsForDfaMatch = (optionsForDfaMatch | PCRE_PARTIAL_HARD);
        }
        if (!iter.isEqualTo(iterator)) {
            optionsForDfaMatch = (optionsForDfaMatch | PCRE_DFA_RESTART);
        }
        int matchCode = pcre_dfa_exec(d->pcreRegexp,
                                      0 /*pcre_study data*/,
                                      fragment.data(), fragment.size(), 0 /* offset */,
                                      optionsForDfaMatch,
                                      captureData, 2,
                                      workspace, 64);
        if (matchCode < 0) {
            if (matchCode == PCRE_ERROR_PARTIAL) {
                // Part of the regexp matched. Continue iterating.
                assert(!iter.isAtEnd());
                iter.moveForwardTillEndOfLine();
                continue;
            } else if (matchCode == PCRE_ERROR_NOMATCH) {
                // Definitely failed to match.
                return false;
            } else {
                // Some other error
                printf("VfmdRegexp: DFA matching failed with PCRE error code: %d.\n", matchCode);
                return false;
            }
        } else {
            // Finished matching
            assert(captureData[0] == 0); // because of PCRE_ANCHORED
            int matchingLength = captureData[1];
            iter.moveForward((unsigned int) matchingLength);
            iterator->moveTo(iter);
            return true;
        }
    }
    // Control should never reach here
    return false;
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
