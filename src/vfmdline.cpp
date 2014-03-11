#include <assert.h>
#include <string.h>
#include "vfmdline.h"

VfmdLine::VfmdLine()
{
}

VfmdLine::VfmdLine(const char *str)
    : VfmdByteArray(str)
{
}

VfmdLine::VfmdLine(const char *data, int length)
    : VfmdByteArray(data, length)
{
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
