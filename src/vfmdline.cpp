#include <assert.h>
#include <string.h>
#include "vfmdline.h"

VfmdLine::VfmdLine(const VfmdByteArray &ba)
    : m_lineContent(ba)
{
}

VfmdByteArray VfmdLine::content() const
{
    return m_lineContent;
}

unsigned int VfmdLine::size() const
{
    return m_lineContent.size();
}

char VfmdLine::firstByte() const
{
    return ((size() > 0)? (*m_lineContent.data()) : 0);
}

bool VfmdLine::isBlankLine() const
{
    const char *data_ptr = m_lineContent.data();
    size_t sz = m_lineContent.size();
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

VfmdLine *VfmdLine::copy() const
{
    return new VfmdLine(m_lineContent);
}

unsigned int VfmdLine::leadingSpacesCount() const
{
    const char *p = m_lineContent.data();
    size_t sz = m_lineContent.size();
    for (unsigned int i = 0; i < sz; i++) {
        if (*p++ != ' ') {
            return i;
        }
    }
    return sz;
}

char VfmdLine::firstNonSpace() const
{
    unsigned int i = leadingSpacesCount();
    if (i < size()) {
        return *(m_lineContent.data() + i);
    }
    return 0;
}

void VfmdLine::chopLeft(unsigned int n)
{
    m_lineContent.chopLeft(n);
}

void VfmdLine::chopRight(unsigned int n)
{
    m_lineContent.chopRight(n);
}
