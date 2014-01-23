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
        if (data_ptr[i] != 0x20 /* space */ && data_ptr[i] != 0x0a /* LF */) {
            return false;
        }
    }
    return true;
}