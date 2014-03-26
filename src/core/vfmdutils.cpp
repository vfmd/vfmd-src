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
