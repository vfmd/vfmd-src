#ifndef VFMDUTILS_H
#define VFMDUTILS_H

#include "vfmdbytearray.h"

// Binary search for 'str' in the given range of 'list'
int indexOfStringInSortedList(const char *str, const char *list[], int offset, int length);

// Locate repeated bytes
// e.g.: byte = 'a', text = "0123aaa789", offset = 0
//       returns: startIndex = 4, count = 3
bool locateByteRepetitionSequence(char byte, const VfmdByteArray &text, unsigned int offset, bool ignoreEscapedBytes,
                                  int *startIndex, int *count);

#endif // VFMDUTILS_H
