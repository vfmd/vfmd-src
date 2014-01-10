#include <stdio.h>
#include "vfmdlinearrayiterator.h"
#include "vfmdlinearray.h"
#include "vfmdline.h"

VfmdLineArrayIterator::VfmdLineArrayIterator(const VfmdLineArray *lineArray, unsigned int lineIndex, unsigned int byteIndex, bool isNextByteEscaped)
    : m_lineArray(lineArray)
    , m_lineIndex(lineIndex)
    , m_byteIndex(byteIndex)
    , m_isNextByteEscaped(isNextByteEscaped)
{
}

VfmdLineArrayIterator* VfmdLineArrayIterator::copy() const
{
    return new VfmdLineArrayIterator(m_lineArray, m_lineIndex, m_byteIndex, m_isNextByteEscaped);
}

VfmdLineArrayIterator::~VfmdLineArrayIterator()
{
}

char VfmdLineArrayIterator::nextByte() const
{
    return m_lineArray->lineAt(m_lineIndex)->charAt(m_byteIndex);
}

bool VfmdLineArrayIterator::isNextByteEscaped() const
{
    return m_isNextByteEscaped;
}

int VfmdLineArrayIterator::numberOfBytesTillEndOfLine() const
{
     return (m_lineArray->lineAt(m_lineIndex)->size() - m_byteIndex);
}

VfmdByteArray *VfmdLineArrayIterator::bytesTillEndOfLine() const
{
    return new VfmdByteArray(m_lineArray->lineAt(m_lineIndex)->mid(m_byteIndex));
}

void VfmdLineArrayIterator::foreachLineSegmentsTill(const VfmdLineArrayIterator *other, void *ctx, bool (*fn)(void *ctx, const VfmdByteArray &ba)) const
{
    if (isBefore(other)) {
        VfmdLineArrayIterator *iter = copy();
        VfmdByteArray ba;
        while ((iter->m_lineIndex < other->m_lineIndex) &&
               (iter->m_lineIndex < m_lineArray->lineCount())) {
            ba = m_lineArray->lineAt(iter->m_lineIndex)->mid(iter->m_byteIndex);
            bool proceed = (*fn)(ctx, ba);
            if (!proceed) {
                delete iter;
                return;
            }
            iter->m_lineIndex++;
            iter->m_byteIndex = 0;
        }
        if (iter->m_lineIndex < m_lineArray->lineCount()) {
            assert(iter->m_lineIndex == other->m_lineIndex);
            ba = m_lineArray->lineAt(iter->m_lineIndex)->mid(iter->m_byteIndex, other->m_byteIndex - iter->m_byteIndex);
            (*fn)(ctx, ba);
        }
        delete iter;
    }
}

static bool addByteArrayLengths(void *ctx, const VfmdByteArray &ba)
{
    *((int *) ctx) += ba.size();
    return true;
}

int VfmdLineArrayIterator::numberOfBytesTill(const VfmdLineArrayIterator *other) const
{
    int distance = 0;
    foreachLineSegmentsTill(other, (void *) &distance, &addByteArrayLengths);
    return distance;
}

static bool concatByteArrays(void *ctx, const VfmdByteArray &ba)
{
    ((VfmdByteArray *) ctx)->append(ba);
    return true;
}

VfmdByteArray* VfmdLineArrayIterator::bytesTill(const VfmdLineArrayIterator *other) const
{
    VfmdByteArray *ba = new VfmdByteArray;
    ba->reserve(numberOfBytesTill(other));
    foreachLineSegmentsTill(other, (void *) ba, &concatByteArrays);
    return ba;
}

void VfmdLineArrayIterator::moveForwardOneByte()
{
    if (m_byteIndex < (m_lineArray->lineAt(m_lineIndex)->size() - 1)) {
        moveTo(m_lineIndex, m_byteIndex + 1);
    } else {
        moveTo(m_lineIndex + 1, 0);
    }
}

void VfmdLineArrayIterator::moveForwardTillEndOfLine()
{
    moveTo(m_lineIndex + 1, 0);
}

static unsigned int numberOfStartingBytesUsingBytesFromString(const VfmdByteArray &ba, const char *str)
{
    register const char *data = ba.data();
    unsigned int length = ba.size();
    register unsigned int matchCount = 0;
    while (matchCount < length) {
        if (strchr(str, (*data)) == 0) {
            // Non-matching byte found
            break;
        }
        matchCount++;
        data++;
    }
    return matchCount;
}

static unsigned int numberOfStartingBytesNotUsingBytesFromString(const VfmdByteArray &ba, const char *str)
{
    register const char *data = ba.data();
    unsigned int length = ba.size();
    register unsigned int matchCount = 0;
    while (matchCount < length) {
        if (strchr(str, (*data)) != 0) {
            // Non-matching byte found
            break;
        }
        matchCount++;
        data++;
    }
    return matchCount;
}

bool advanceOverMatchingBytes(void *ctx, const VfmdByteArray &ba)
{
    struct VfmdLineArrayIterator::ForeachLineSegmentsStringMatchingContext *context = (struct VfmdLineArrayIterator::ForeachLineSegmentsStringMatchingContext *) ctx;
    unsigned int byteCount = numberOfStartingBytesUsingBytesFromString(ba, context->str);
    if (byteCount == ba.size()) {
        context->lineIndex++;
        context->byteIndex = 0;
        return true;
    } else {
        context->byteIndex += byteCount;
        return false;
    }
}

bool advanceOverNonMatchingBytes(void *ctx, const VfmdByteArray &ba)
{
    struct VfmdLineArrayIterator::ForeachLineSegmentsStringMatchingContext *context = (struct VfmdLineArrayIterator::ForeachLineSegmentsStringMatchingContext *) ctx;
    unsigned int byteCount = numberOfStartingBytesNotUsingBytesFromString(ba, context->str);
    if (byteCount == ba.size()) {
        context->lineIndex++;
        context->byteIndex = 0;
        return true;
    } else {
        context->byteIndex += byteCount;
        return false;
    }
}

void VfmdLineArrayIterator::moveForwardOverBytesInString(const char *str)
{
    ForeachLineSegmentsStringMatchingContext context(m_lineIndex, m_byteIndex, str);
    foreachLineSegmentsTill(m_lineArray->end(), &context, &advanceOverMatchingBytes);
    moveTo(context.lineIndex, context.byteIndex);
}

void VfmdLineArrayIterator::moveForwardOverBytesNotInString(const char *str)
{
    ForeachLineSegmentsStringMatchingContext context(m_lineIndex, m_byteIndex, str);
    foreachLineSegmentsTill(m_lineArray->end(), &context, &advanceOverNonMatchingBytes);
    moveTo(context.lineIndex, context.byteIndex);
}

void VfmdLineArrayIterator::moveTo(const VfmdLineArrayIterator *other)
{
    if (m_lineArray == other->m_lineArray) {
        m_lineIndex = other->m_lineIndex;
        m_byteIndex = other->m_byteIndex;
        m_isNextByteEscaped = other->m_isNextByteEscaped;
    }
}

static bool isEscapedAtEndOfString(bool startIsEscaped, const char *str, unsigned int len)
{
    if (len == 0) {
        return startIsEscaped;
    }
    assert(len > 0);
    if (str[len - 1] != '\\') {
        return false;
    }
    bool isEscaped = startIsEscaped;
    while (len--) {
        if (isEscaped) {
            isEscaped = false;
        } else if (*str == '\\') {
            isEscaped = true;
        }
        str++;
    }
    return isEscaped;
}

void VfmdLineArrayIterator::moveTo(unsigned int otherLineIndex, unsigned int otherByteIndex)
{
    if (m_lineIndex == otherLineIndex) {
        // Same line
        if (m_byteIndex == otherByteIndex) {
            // Nothing to do
            return;
        }
        const VfmdLine *line = m_lineArray->lineAt(m_lineIndex);
        assert(m_byteIndex < line->size());
        assert(otherByteIndex < line->size());
        if (m_byteIndex < otherByteIndex) {
            m_isNextByteEscaped = isEscapedAtEndOfString(m_isNextByteEscaped, line->data() + m_byteIndex, otherByteIndex - m_byteIndex);
        } else {
            m_isNextByteEscaped = isEscapedAtEndOfString(false, line->data(), otherByteIndex);
        }
    } else {
        if (otherLineIndex >= m_lineArray->lineCount()) {
            // Past the end
            m_lineIndex = otherLineIndex;
            m_byteIndex = otherByteIndex;
            m_isNextByteEscaped = false;
            return;
        }
        const VfmdLine *line = m_lineArray->lineAt(otherLineIndex);
        assert(otherByteIndex < line->size());
        m_isNextByteEscaped = isEscapedAtEndOfString(false, line->data(), otherByteIndex);
    }
    m_lineIndex = otherLineIndex;
    m_byteIndex = otherByteIndex;
}

bool VfmdLineArrayIterator::isAtEnd() const
{
    return (m_lineIndex >= m_lineArray->lineCount());
}

bool VfmdLineArrayIterator::isEqualTo(const VfmdLineArrayIterator *other) const
{
    return ((m_lineArray == other->m_lineArray) &&
            (m_lineIndex == other->m_lineIndex) &&
            (m_byteIndex == other->m_byteIndex));
}

bool VfmdLineArrayIterator::isBefore(const VfmdLineArrayIterator *other) const
{
    return ((m_lineArray == other->m_lineArray) &&
            ((m_lineIndex < other->m_lineIndex) || (m_lineIndex == other->m_lineIndex && m_byteIndex < other->m_byteIndex)));
}

bool VfmdLineArrayIterator::isAfter(const VfmdLineArrayIterator *other) const
{
    return ((m_lineArray == other->m_lineArray) &&
            ((m_lineIndex > other->m_lineIndex) || (m_lineIndex == other->m_lineIndex && m_byteIndex > other->m_byteIndex)));
}

bool VfmdLineArrayIterator::operator ==(const VfmdLineArrayIterator &other) const
{
    return isEqualTo(&other);
}

bool VfmdLineArrayIterator::operator <(const VfmdLineArrayIterator &other) const
{
    return isBefore(&other);
}

bool VfmdLineArrayIterator::operator >(const VfmdLineArrayIterator &other) const
{
    return isAfter(&other);
}
