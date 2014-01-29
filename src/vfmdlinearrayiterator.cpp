#include <stdio.h>
#include "vfmdlinearrayiterator.h"
#include "vfmdlinearray.h"
#include "vfmdline.h"
#include "vfmdscopedpointer.h"

VfmdLineArrayIterator::VfmdLineArrayIterator()
    : m_lineArray(0)
    , m_lineIndex(0)
    , m_byteIndex(0)
    , m_isNextByteEscaped(false)
{
}

bool VfmdLineArrayIterator::isValid() const
{
    return (m_lineArray != 0);
}

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

char VfmdLineArrayIterator::previousByte() const
{
    if (m_byteIndex > 0) {
        // Return the previous byte in the same line
        return m_lineArray->lineAt(m_lineIndex)->charAt(m_byteIndex - 1);
    } else if (m_lineIndex > 0) {
        // Return the last byte of the previous line
        return m_lineArray->lineAt(m_lineIndex - 1)->lastChar();
    }
    return 0;
}

bool VfmdLineArrayIterator::isNextByteEscaped() const
{
    return m_isNextByteEscaped;
}

int VfmdLineArrayIterator::numberOfBytesTillEndOfLine() const
{
     return (m_lineArray->lineAt(m_lineIndex)->size() - m_byteIndex);
}

VfmdByteArray VfmdLineArrayIterator::bytesTillEndOfLine() const
{
    return m_lineArray->lineAt(m_lineIndex)->mid(m_byteIndex);
}

void VfmdLineArrayIterator::foreachLineSegmentsTill(const VfmdLineArrayIterator &other, void *ctx, bool (*fn)(void *ctx, const VfmdByteArray &ba)) const
{
    if (isBefore(&other)) {
        VfmdLineArrayIterator iter = (*this);
        VfmdByteArray ba;
        while ((iter.m_lineIndex < other.m_lineIndex) &&
               (iter.m_lineIndex < m_lineArray->lineCount())) {
            ba = m_lineArray->lineAt(iter.m_lineIndex)->mid(iter.m_byteIndex);
            bool proceed = (*fn)(ctx, ba);
            if (!proceed) {
                return;
            }
            iter.m_lineIndex++;
            iter.m_byteIndex = 0;
        }
        if (iter.m_lineIndex < m_lineArray->lineCount()) {
            assert(iter.m_lineIndex == other.m_lineIndex);
            ba = m_lineArray->lineAt(iter.m_lineIndex)->mid(iter.m_byteIndex, other.m_byteIndex - iter.m_byteIndex);
            (*fn)(ctx, ba);
        }
    }
}

static bool addByteArrayLengths(void *ctx, const VfmdByteArray &ba)
{
    *((int *) ctx) += ba.size();
    return true;
}

int VfmdLineArrayIterator::numberOfBytesTill(const VfmdLineArrayIterator &other) const
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

VfmdByteArray VfmdLineArrayIterator::bytesTill(const VfmdLineArrayIterator &other) const
{
    VfmdByteArray ba;
    ba.reserve(numberOfBytesTill(other));
    foreachLineSegmentsTill(other, (void *) &ba, &concatByteArrays);
    return ba;
}


void VfmdLineArrayIterator::moveForward(unsigned int n)
{
    if (n < (unsigned int) numberOfBytesTillEndOfLine()) {
        // Within the same line
        moveTo(m_lineIndex, m_byteIndex + n);
    } else {
        int lineIndex = m_lineIndex;
        int byteIndex = m_byteIndex;
        int lineLength = m_lineArray->lineAt(lineIndex)->size();
        int delta = n;
        while ((byteIndex + delta) >= lineLength) {
            delta -= (lineLength - byteIndex);
            lineIndex++;
            byteIndex = 0;
            if (lineIndex >= int(m_lineArray->lineCount())) {
                lineLength = 0;
                break;
            }
            lineLength = m_lineArray->lineAt(lineIndex)->size();
        }
        byteIndex += delta;
        if (lineIndex >= int(m_lineArray->lineCount())) {
            lineIndex = m_lineArray->lineCount();
            byteIndex = 0;
        }
        moveTo(lineIndex, byteIndex);
    }
}

void VfmdLineArrayIterator::moveBackward(unsigned int n)
{
    if (n <= m_byteIndex) {
        // Within the same line
        moveTo(m_lineIndex, m_byteIndex - n);
    } else {
        int lineIndex = (int) m_lineIndex;
        int byteIndex = (int) m_byteIndex;
        int delta = n;
        while (delta > byteIndex) {
            if (lineIndex < 0) {
                break;
            }
            delta -= byteIndex;
            lineIndex--;
            byteIndex = m_lineArray->lineAt(lineIndex)->size() - 1;
            delta--;
        }
        byteIndex -= delta;
        if (lineIndex < 0) {
            lineIndex = 0;
            byteIndex = 0;
        }
        moveTo(lineIndex, byteIndex);
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

bool VfmdLineArrayIterator::moveForwardOverBytesInString(const char *str)
{
    ForeachLineSegmentsStringMatchingContext context(m_lineIndex, m_byteIndex, str);
    foreachLineSegmentsTill(m_lineArray->end(), &context, &advanceOverMatchingBytes);
    if (m_lineIndex == context.lineIndex && m_byteIndex == context.byteIndex) {
        return false;
    }
    moveTo(context.lineIndex, context.byteIndex);
    return true;
}

bool VfmdLineArrayIterator::moveForwardOverBytesNotInString(const char *str)
{
    ForeachLineSegmentsStringMatchingContext context(m_lineIndex, m_byteIndex, str);
    foreachLineSegmentsTill(m_lineArray->end(), &context, &advanceOverNonMatchingBytes);
    if (m_lineIndex == context.lineIndex && m_byteIndex == context.byteIndex) {
        return false;
    }
    moveTo(context.lineIndex, context.byteIndex);
    return true;
}

bool VfmdLineArrayIterator::moveForwardOverByteSequence(const char *str, int len)
{
    if (len == 0) {
        len = strlen(str);
    }
    bool matched = true;
    VfmdLineArrayIterator iter = (*this);
    for (int i = 0; i < len; i++) {
        if (iter.isAtEnd()) {
            matched = false;
            break;
        }
        char c = iter.nextByte();
        iter.moveForward(1);
        if (c != str[len]) {
            matched = false;
            break;
        }
    }
    if (matched) {
        moveTo(iter);
    }
    return matched;
}

bool VfmdLineArrayIterator::moveForwardOverRegexp(const VfmdRegexp &regexp)
{
    return regexp.moveIteratorForward(this);
}

bool VfmdLineArrayIterator::moveTo(const VfmdLineArrayIterator &other)
{
    if (m_lineArray == other.m_lineArray) {
        m_lineIndex = other.m_lineIndex;
        m_byteIndex = other.m_byteIndex;
        m_isNextByteEscaped = other.m_isNextByteEscaped;
        return true;
    }
    return false;
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

bool VfmdLineArrayIterator::isAtBeginning() const
{
    return (m_lineIndex == 0 && m_byteIndex == 0);
}

bool VfmdLineArrayIterator::isAtEnd() const
{
    return (m_lineIndex >= m_lineArray->lineCount());
}

bool VfmdLineArrayIterator::isAtLastLine() const
{
    return (m_lineIndex >= (m_lineArray->lineCount() - 1));
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

bool VfmdLineArrayIterator::operator <=(const VfmdLineArrayIterator &other) const
{
    return (isBefore(&other) || isEqualTo(&other));
}

bool VfmdLineArrayIterator::operator >=(const VfmdLineArrayIterator &other) const
{
    return (isAfter(&other) || isEqualTo(&other));
}
