#include <assert.h>
#include <string.h>
#include "vfmdline.h"

VfmdLine::VfmdLine()
{
}

VfmdLine::VfmdLine(const char *str)
    : VfmdByteArray(str)
{
    // Assert that the last byte is an LF
    assert((strlen(str) > 0) && (str[strlen(str) - 1] == 0x0a /* LF */));
}

VfmdLine::VfmdLine(const char *data, int length)
    : VfmdByteArray(data, length)
{
    // Assert that the last byte is an LF
    assert((length > 0) && (data[length - 1] == 0x0a /* LF */));
}

bool VfmdLine::isBlankLine() const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (unsigned int i = 0; i < sz; i++) {
        const char c = data_ptr[i];
        if (c != 0x09 /* Tab */ &&
            c != 0x0a /* LF */ &&
            c != 0x0c /* FF */ &&
            c != 0x0d /* CR */ &&
            c != 0x20 /* Space */) {
            return false;
        }
    }
    return true;
}
