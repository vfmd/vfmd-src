#include "vfmdline.h"

VfmdLine::VfmdLine()
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
        if (data_ptr[i] != 0x20 /* space */ && data_ptr[i] != 0x0a /* LF */) {
            return false;
        }
    }
    return true;
}
