#include "core/vfmdutils.h"
#include <string.h>
#include <assert.h>

int indexOfStringInSortedList(const char *str, const char *list[], int offset, int length)
{
    if (length <= 0) {
        return -1;
    }
    int mid = (offset + length / 2);
    int cmp = strcmp(str, list[mid]);
    if (cmp < 0) {
        return indexOfStringInSortedList(str, list, offset, mid - offset);
    } else if (cmp > 0) {
        return indexOfStringInSortedList(str, list, mid + 1, offset + length - mid - 1);
    }
    assert(cmp == 0);
    return mid;
}

bool locateByteRepetitionSequence(char candidateByte, const VfmdByteArray &text, unsigned int offset,  bool ignoreEscapedBytes,
                                  int *startIndex, int *count)
{
    const char *p = text.data() + offset;
    unsigned int sz = text.size();

    int indexOfNextCandidateByte = -1;
    for (unsigned int i = offset; i < sz; i++, p++) {
        if ((*p == candidateByte) && (!ignoreEscapedBytes || !text.isEscapedAtPosition(i))) {
            indexOfNextCandidateByte = i;
            break;
        }
    }

    if (indexOfNextCandidateByte >= 0) {
        (*startIndex) = indexOfNextCandidateByte;
        for (unsigned int i = indexOfNextCandidateByte; i < sz; i++, p++) {
            if (*p != candidateByte) {
                (*count) = (i - indexOfNextCandidateByte);
                return true;
            }
        }
        (*count) = (sz - indexOfNextCandidateByte);
        return true;
    }

    return false;
}
