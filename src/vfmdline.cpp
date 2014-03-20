#include <assert.h>
#include <string.h>
#include "vfmdline.h"

VfmdLine::VfmdLine(const VfmdByteArray &ba)
    : m_lineContent(ba)
    , m_isLineDataPrecomputed(false)
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

VfmdLine *VfmdLine::copy() const
{
    return new VfmdLine(m_lineContent);
}

static unsigned int leadingSpacesCountInString(const char *p, unsigned int sz)
{
    for (unsigned int i = 0; i < sz; i++) {
        if (*p++ != ' ') {
            return i;
        }
    }
    return sz;
}

static char firstNonSpaceInString(const char *p, unsigned int sz)
{
    unsigned int i = leadingSpacesCountInString(p, sz);
    if (i < sz) {
        return p[i];
    }
    return 0;
}

static bool isAllBytesWhitespace(const unsigned char *p, int sz)
{
    while (sz--) {
        const unsigned char c = *p++;
        assert(c != 0x0a /* LF */);
        assert(c != 0x09 /* Tab */);
        if ((c != 0x20) /* space */ &&
            ((c & 0xfe) != 0x0c) /* FF, CR */) {
            return false;
        }
    }
    return true;
}

void VfmdLine::precomputeLineData()
{
    if (m_isLineDataPrecomputed) {
        return;
    }
    const unsigned char *p = reinterpret_cast<const unsigned char *>(m_lineContent.data());
    size_t sz = m_lineContent.size();
    while (sz--) {
        const unsigned char c = *p++;

        assert(c != 0x0a /* LF */);
        assert(c != 0x09 /* Tab */);

        if (c != 0x20 /* Space */) {
            m_leadingSpacesCount = (m_lineContent.size() - sz - 1);
            if ((c & 0xfe) == 0x0c /* FF or CR */) {
                m_isBlankLine = isAllBytesWhitespace(p, sz);
            } else {
                m_isBlankLine = false;
            }
            m_isLineDataPrecomputed = true;
            return;
        }
    }
    m_leadingSpacesCount = m_lineContent.size();
    m_isBlankLine = true;
    m_isLineDataPrecomputed = true;
}

char VfmdLine::firstNonSpace() const
{
    if (size() == 0) {
        return 0;
    }
    if (m_isLineDataPrecomputed) {
        return (m_lineContent.data()[m_leadingSpacesCount]);
    }
    return (firstNonSpaceInString(m_lineContent.data(), m_lineContent.size()));
}

unsigned int VfmdLine::leadingSpacesCount() const
{
    if (m_isLineDataPrecomputed) {
        return m_leadingSpacesCount;
    }
    return (leadingSpacesCountInString(m_lineContent.data(), m_lineContent.size()));
}

bool VfmdLine::isBlankLine() const
{
    if (m_isLineDataPrecomputed) {
        return m_isBlankLine;
    }
    return (isAllBytesWhitespace(reinterpret_cast<const unsigned char *>(m_lineContent.data()), m_lineContent.size()));
}

void VfmdLine::chopLeft(unsigned int n)
{
    m_lineContent.chopLeft(n);
    m_isLineDataPrecomputed = false;
}

void VfmdLine::chopRight(unsigned int n)
{
    m_lineContent.chopRight(n);
    m_isLineDataPrecomputed = false;
}

int VfmdLine::indexOf(const VfmdRegexp& re) const
{
    return re.locateInBytearrayWithoutCapturing(m_lineContent);
}

bool VfmdLine::matches(const VfmdRegexp& re) const
{
    return (re.locateInBytearrayWithoutCapturing(m_lineContent) >= 0);
}
