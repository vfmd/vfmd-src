#include "vfmdbytearray.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define DEFAULT_REALLOC_CHUNK_SIZE (128)

class VfmdByteArray::Private {
public:
    Private() : refCount(1), data(0), size(0), allocatedSize(0) {}

    Private(const char *str, size_t len, size_t reservedSpace = 0) : refCount(1) {
        if (reservedSpace < len) {
            reservedSpace = len;
        }
        data = static_cast<char *>(malloc(reservedSpace));
        allocatedSize = reservedSpace;
        memcpy(data, str, len);
        size = len;
    }

    ~Private() {
        free(data);
    }

    void reallocateBuffer(size_t requestedAllocatedSize) {
        if (requestedAllocatedSize < size) {
            // Would lose data. Don't reallocate.
            return;
        }
        if (requestedAllocatedSize == allocatedSize) {
            // Nothing to do.
            return;
        }
        data = static_cast<char *>(realloc(data, requestedAllocatedSize));
        allocatedSize = requestedAllocatedSize;
    }

    void increaseReserveMemory(size_t minAdditionalSpace) {
        size_t requiredSize = (size + minAdditionalSpace);
        if (requiredSize > allocatedSize) {
            // Allocate memory in chunks
            size_t chunkSize = (size? size : DEFAULT_REALLOC_CHUNK_SIZE);
            reallocateBuffer((size_t(requiredSize / chunkSize) + 1) * chunkSize);
        }
    }

    void append(const char *str, size_t len) {
        increaseReserveMemory(len);
        memcpy(data + size, str, len);
        size += len;
    }

    void appendByte(char byte, int ntimes = 1) {
        increaseReserveMemory(ntimes);
        while (ntimes--) {
            data[size++] = byte;
        }
    }

    unsigned int refCount;

    char *data;
    size_t size;
    size_t allocatedSize;
};

VfmdByteArray::VfmdByteArray()
    : d(new Private()), m_offset(0), m_length(0)
{
    assert(d);
    assert(d->refCount);
}

VfmdByteArray::VfmdByteArray(const char *str)
{
    int len = strlen(str);
    d = new Private(str, len);
    m_offset = 0;
    m_length = len;
    assert(d);
    assert(d->refCount);
}

VfmdByteArray::VfmdByteArray(const char *data, int length)
    : d(new Private(data, length)), m_offset(0), m_length(length)
{
    assert(d);
    assert(d->refCount);
}

VfmdByteArray::~VfmdByteArray()
{
    assert(d);
    assert(d->refCount);
    deref();
}

bool VfmdByteArray::isValid() const
{
    return (d->data != 0);
}

bool VfmdByteArray::isInvalid() const
{
    return (d->data == 0);
}

void VfmdByteArray::invalidate()
{
    deref();
    d = new Private();
}

void VfmdByteArray::append(const char *data, int length)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(length);
    }
    d->append(data, length);
    m_length += length;
}

void VfmdByteArray::append(const VfmdByteArray &other)
{
    append(other.data(), other.size());
}

void VfmdByteArray::appendByte(char byte1)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(1);
    }
    d->appendByte(byte1);
    m_length++;
}

void VfmdByteArray::appendBytes(char byte1, char byte2)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(2);
    }
    d->appendByte(byte1);
    d->appendByte(byte2);
    m_length += 2;
}

void VfmdByteArray::appendBytes(char byte1, char byte2, char byte3)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(3);
    }
    d->appendByte(byte1);
    d->appendByte(byte2);
    d->appendByte(byte3);
    m_length += 3;
}

void VfmdByteArray::appendBytes(char byte1, char byte2, char byte3, char byte4)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(4);
    }
    d->appendByte(byte1);
    d->appendByte(byte2);
    d->appendByte(byte3);
    d->appendByte(byte4);
    m_length += 4;
}

void VfmdByteArray::appendByteNtimes(char byte, int n)
{
    if ((m_offset + m_length) < d->size) {
        copyOnWrite(n);
    }
    d->appendByte(byte, n);
    m_length += n;
}

bool VfmdByteArray::chopLeft(unsigned int count)
{
    if (count > m_length) {
        return false;
    }
    m_offset += count;
    m_length -= count;
    return true;
}

bool VfmdByteArray::chopRight(unsigned int count)
{
    if (count > m_length) {
        return false;
    }
    if (d->refCount == 1) {
        // No other instance is using the same data.
        // We can modify d.
        if ((m_offset + m_length) < d->size) {
            d->size = (m_offset + m_length);
        }
        assert((m_offset + m_length) == d->size);
        d->size -= count;
    }
    m_length -= count;
    return true;
}

const char* VfmdByteArray::data() const
{
    return (d->data + m_offset);
}

size_t VfmdByteArray::size() const
{
    return m_length;
}

char VfmdByteArray::byteAt(unsigned int pos) const
{
    return *(data() + pos);
}

char VfmdByteArray::lastByte() const
{
    return byteAt(size() - 1);
}

bool VfmdByteArray::startsWith(const char *str) const
{
    const char *data_ptr = data();
    size_t sz = size();
    unsigned int i;
    for (i = 0; i < sz; i++) {
        const char c = str[i];
        if (c == 0) {
            return true;
        }
        if (data_ptr[i] != c) {
            return false;
        }
    }
    if (str[i] == 0) {
        // strings are equal
        return true;
    }
    // str has more unmatched bytes
    return false;
}

bool VfmdByteArray::startsWith(const VfmdByteArray &ba) const
{
    if (!ba.isValid()) {
        return false;
    }
    const char *data_ptr = data();
    size_t sz = size();
    size_t subjectSize = ba.size();
    unsigned int i;
    for (i = 0; i < sz; i++) {
        if (i == subjectSize) {
            return true;
        }
        const char c = ba.byteAt(i);
        if (data_ptr[i] != c) {
            return false;
        }
    }
    if (i == subjectSize) {
        // strings are equal
        return true;
    }
    // ba has more unmatched bytes
    return false;
}

char VfmdByteArray::firstNonSpace() const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (unsigned int i = 0; i < sz; i++) {
        if (data_ptr[i] != 0x20 /* space */) {
            return data_ptr[i];
        }
    }
    return 0;
}

char VfmdByteArray::lastNonSpace() const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (int i = (int) sz - 1; i >= 0; i--) {
        if (data_ptr[i] != 0x20 /* space */) {
            return data_ptr[i];
        }
    }
    return 0;
}

int VfmdByteArray::indexOfFirstNonSpace() const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (unsigned int i = 0; i < sz; i++) {
        if (data_ptr[i] != 0x20 /* space */) {
            return (int) i;
        }
    }
    return -1;
}

int VfmdByteArray::indexOfLastNonSpace() const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (int i = (int) sz - 1; i >= 0; i--) {
        if (data_ptr[i] != 0x20 /* space */) {
            return (int) i;
        }
    }
    return -1;
}

int VfmdByteArray::indexOf(char byte, int offset) const
{
    const char *p = data() + offset;
    size_t sz = size();
    for (unsigned int i = offset; i < sz; i++) {
        if (*p++ == byte) {
            return (int) i;
        }
    }
    return -1;
}

bool VfmdByteArray::isEscapedAtPosition(int pos) const
{
    if (pos <= 0) {
        return false;
    }
    assert(pos > 0);
    assert(pos < size());
    const char *data_ptr = data();
    unsigned int numOfPrecedingBackslashes = 0;
    int i = pos - 1;
    while ((i >= 0) && (data_ptr[i] == '\\')) {
        i--;
        numOfPrecedingBackslashes++;
    }
    return ((numOfPrecedingBackslashes % 2) == 1);
}

VfmdByteArray VfmdByteArray::left(unsigned int count) const
{
    VfmdByteArray ba = *this;
    ba.chopRight(ba.size() - count);
    return ba;
}

VfmdByteArray VfmdByteArray::right(unsigned int count) const
{
    VfmdByteArray ba = *this;
    ba.chopLeft(ba.size() - count);
    return ba;
}

VfmdByteArray VfmdByteArray::mid(unsigned int n) const
{
    VfmdByteArray ba = *this;
    ba.chopLeft(n);
    return ba;
}

VfmdByteArray VfmdByteArray::mid(unsigned int n, unsigned int l) const
{
    VfmdByteArray ba = *this;
    ba.chopLeft(n);
    ba.chopRight(ba.size() - l);
    return ba;
}

void VfmdByteArray::trimLeft()
{
    const char *data_ptr = data();
    size_t sz = size();
    unsigned int i;
    for (i = 0; i < sz; i++) {
        char c = data_ptr[i];
        if (c != 0x09 /* Tab */ &&
            c != 0x0a /* LF */ &&
            c != 0x0c /* FF */ &&
            c != 0x0d /* CR */ &&
            c != 0x20 /* Space */) {
            break;
        }
    }
    unsigned int bytesToChop = i;
    if (bytesToChop > 0) {
        chopLeft(bytesToChop);
    }
}

void VfmdByteArray::trimRight()
{
    const char *data_ptr = data();
    size_t sz = size();
    unsigned int i;
    for (i = sz; i > 0; i--) {
        char c = data_ptr[i - 1];
        if (c != 0x09 /* Tab */ &&
            c != 0x0a /* LF */ &&
            c != 0x0c /* FF */ &&
            c != 0x0d /* CR */ &&
            c != 0x20 /* Space */) {
            break;
        }
    }
    unsigned int bytesToChop = (sz - i);
    if (bytesToChop > 0) {
        chopRight(bytesToChop);
    }
}

void VfmdByteArray::trim()
{
    trimRight();
    trimLeft();
}

VfmdByteArray VfmdByteArray::leftTrimmed() const
{
    VfmdByteArray implicitCopy = *(this);
    implicitCopy.trimLeft();
    return implicitCopy;
}

VfmdByteArray VfmdByteArray::rightTrimmed() const
{
    VfmdByteArray implicitCopy = *(this);
    implicitCopy.trimRight();
    return implicitCopy;
}

VfmdByteArray VfmdByteArray::trimmed() const
{
    VfmdByteArray implicitCopy = *(this);
    implicitCopy.trimRight();
    implicitCopy.trimLeft();
    return implicitCopy;
}

VfmdByteArray VfmdByteArray::simplified() const
{
    VfmdByteArray ba;
    VfmdByteArray trimmedSelf = trimmed();
    ba.reserve(trimmedSelf.size());

    const char *data_ptr = trimmedSelf.data();
    size_t sz = trimmedSelf.size();
    bool isInWhitespaceStretch;

    bool isAlreadySimplified = true;
    isInWhitespaceStretch = false;
    for (unsigned int i = 0; i < sz; i++) {
        char c = data_ptr[i];
        if (c == 0x09 /* Tab */ ||
            c == 0x0a /* LF */ ||
            c == 0x0c /* FF */ ||
            c == 0x0d /* CR */ ||
            c == 0x20 /* Space */) {
            if (isInWhitespaceStretch || (c != 0x20)) {
                isAlreadySimplified = false;
                break;
            }
            isInWhitespaceStretch = true;
        } else {
            isInWhitespaceStretch = false;
        }
    }

    if (isAlreadySimplified) {
        return trimmedSelf;
    }

    isInWhitespaceStretch = false;
    for (unsigned int i = 0; i < sz; i++) {
        char c = data_ptr[i];
        if (c == 0x09 /* Tab */ ||
            c == 0x0a /* LF */ ||
            c == 0x0c /* FF */ ||
            c == 0x0d /* CR */ ||
            c == 0x20 /* Space */) {
            if (!isInWhitespaceStretch) {
                ba.appendByte(0x20 /* Space */);
                isInWhitespaceStretch = true;
            }
        } else {
            ba.appendByte(c);
            isInWhitespaceStretch = false;
        }
    }
    return ba;
}

VfmdByteArray VfmdByteArray::bytesInStringRemoved(const char *bytesToRemove) const
{
    const char *data_ptr = data();
    size_t sz = size();
    int n = strlen(bytesToRemove);
    bool isByteToBeRemoved;

    int indexOfFirstByteToRemove = -1;
    for (unsigned int i = 0; i < sz; i++) {
        char c = data_ptr[i];
        isByteToBeRemoved = false;
        for (int j = 0; j < n; j++) {
            if (c == bytesToRemove[j]) {
                isByteToBeRemoved = true;
                break;
            }
        }
        if (isByteToBeRemoved) {
            indexOfFirstByteToRemove = i;
            break;
        }
    }

    if (indexOfFirstByteToRemove < 0) {
        return *(this);
    }

    VfmdByteArray ba;
    ba.reserve(sz);
    if (indexOfFirstByteToRemove > 0) {
        ba.append(data_ptr, indexOfFirstByteToRemove);
    }
    for (unsigned int i = indexOfFirstByteToRemove + 1; i < sz; i++) {
        char c = data_ptr[i];
        isByteToBeRemoved = false;
        for (int j = 0; j < n; j++) {
            if (c == bytesToRemove[j]) {
                isByteToBeRemoved = true;
                break;
            }
        }
        if (!isByteToBeRemoved) {
            ba.appendByte(c);
        }
    }
    return ba;
}

void VfmdByteArray::chomp()
{
    if (lastByte() == '\n') {
        chopRight(1);
    }
}

VfmdByteArray VfmdByteArray::chomped() const
{
    VfmdByteArray ba = *(this);
    ba.chomp();
    return ba;
}

void VfmdByteArray::reserve(size_t length)
{
    if (d->allocatedSize < length) {
        d->reallocateBuffer(length);
    }
}

void VfmdByteArray::reserveAdditionalBytes(size_t minAdditionalSpace)
{
    d->increaseReserveMemory(minAdditionalSpace);
}

size_t VfmdByteArray::capacity() const
{
    return d->allocatedSize;
}

void VfmdByteArray::clear()
{
    assert(d);
    assert(d->refCount);
    if (d->refCount > 1) {
        // Some other instance is using the same data.
        deref();
        d = new Private();
    } else {
        // No other instance is using the same data.
        d->size = 0;
    }
    m_offset = 0;
    m_length = 0;
    assert(d);
    assert(d->refCount);
}

// Implicit sharing stuff follows

VfmdByteArray::VfmdByteArray(const VfmdByteArray &other)
    : d(other.d) {
    assert(d);
    assert(d->refCount);
    ref();
    m_offset = other.m_offset;
    m_length = other.m_length;
    assert(d);
    assert(d->refCount);
}

VfmdByteArray& VfmdByteArray::operator=(const VfmdByteArray &other) {
    assert(d);
    assert(d->refCount);
    if (this != &other) {
        deref(); // dereference existing data
        d = other.d;
        ref();   // reference new data
        m_offset = other.m_offset;
        m_length = other.m_length;
    }
    assert(d);
    assert(d->refCount);
    return *this;
}

void VfmdByteArray::ref() {
    assert(d);
    assert(d->refCount);
    d->refCount++;
}

void VfmdByteArray::deref() {
    assert(d);
    assert(d->refCount);
    d->refCount--;
    if (d->refCount == 0) {
        delete d;
        d = 0;
        m_offset = 0;
        m_length = 0;
    }
}

void VfmdByteArray::copyOnWrite(size_t additionalSpaceRequired) {
    assert(d);
    assert(d->refCount);
    if (d->refCount > 1) {
        // There are other instances that use this data,
        // so make a copy of the data.
        deref(); // dereference existing data
        // copy data (refCount will be 1 for new copy)
        d = new Private(data(), size(), size() + additionalSpaceRequired);
        m_offset = 0;
    } else if (d->refCount == 1) {
        // This is the only copy that uses this data,
        // so use this opportunity to trim any unused bytes
        // at the right end of our data storage.
        if ((m_offset + m_length) < d->size) {
            d->size = (m_offset + m_length);
        }
    }
    assert(d);
    assert(d->refCount);
    assert((m_offset + m_length) == d->size);
}

VfmdByteArray *VfmdByteArray::copy() const
{
    return new VfmdByteArray(*this);
}

VfmdByteArray VfmdByteArray::clone() const
{
    return VfmdByteArray(data(), size());
}

void VfmdByteArray::squeeze()
{
    assert(d);
    assert(d->refCount);
    if (d->refCount > 1) {
        // Some other instance is using the same data.
        // Let's make a copy of the data, so that the other instance is not affected.
        deref();
        d = new Private(data(), size());
        m_offset = 0;
        assert(d);
        assert(d->refCount);
    } else {
        // No other instance is using the same data.
        // We can realloc the data we hold.
        assert(d->refCount == 1);
        if (m_offset > 0) {
            memmove(d->data, d->data + m_offset, m_length);
            m_offset = 0;
        }
        d->size = m_length;
        d->reallocateBuffer(m_length);
    }
    assert(size() == capacity());
}

bool VfmdByteArray::isEqualTo(const char *str, int len) const
{
    if (len == 0) {
        len = strlen(str);
    }
    if (!isValid()) {
        return false;
    }
    if (size() != len) {
        return false;
    }
    if (d->data == str) {
        return true;
    }
    for (unsigned int i = 0; i < size(); i++) {
        if (byteAt(i) != str[i]) {
            return false;
        }
    }
    return true;
}

bool VfmdByteArray::isEqualTo(const VfmdByteArray &other) const
{
    if (isValid() != other.isValid()) {
        return false;
    }
    return isEqualTo(other.data(), other.size());
}

bool VfmdByteArray::operator==(const VfmdByteArray &other) const
{
    if (isValid() != other.isValid()) {
        return false;
    }
    if (size() != other.size()) {
        return false;
    }
    if (size() == 0 && other.size() == 0) {
        return true;
    }
    return isEqualTo(other.data(), other.size());
}

// Printing helper

const char* VfmdByteArray::c_str() const
{
    size_t len = size();
    if (len == 0) {
        return "";
    }
    if (lastByte() == '\n') {
        len++;
    }
    char *str = static_cast<char *>(malloc(len + 1));
    memcpy(str, data(), len);
    if (lastByte() == '\n') {
        str[len - 2] = '\\';
        str[len - 1] = 'n';
    }
    str[len] = '\0';
    return str;
}

void VfmdByteArray::print() const
{
    const char *data_ptr = data();
    size_t sz = size();
    if (data_ptr) {
        for (unsigned int i = 0; i < sz; i++) {
            if (data_ptr[i] == '\n') {
                printf("\\n");
            }
            printf("%c", data_ptr[i]);
        }
    }
}

/* The following unicode related functionality is adapted from
 * the PCRE project (http://www.pcre.org/)
 */

/*
-----------------------------------------------------------------------------
                       Written by Philip Hazel
           Copyright (c) 1997-2013 University of Cambridge

                 Adapted for vfmd by Roopesh Chander
          Copyright (c) 2013 Roopesh Chander <roop@roopc.net>
-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#define PRIV(name) _pcre_##name

// Following code from pcre_internal.h

/* Tells the biggest code point which can be encoded as a single character. */

#define MAX_VALUE_FOR_SINGLE_BYTE_UTF8_CHAR 127

/* Tests whether the code point needs extra characters to decode. */

#define HAS_ADDITIONAL_UTF8_BYTES(c) ((c) >= 0xc0)

/* Returns with the additional number of characters if
 * HAS_ADDITIONAL_UTF8_BYTES(c) is TRUE. Otherwise it has an undefined behaviour. */

#define NUMBER_OF_ADDITIONAL_UTF8_BYTES(c) (PRIV(utf8_table4)[(c) & 0x3f])

/* Returns TRUE, if the given character is the first character
of a UTF sequence. */

#define IS_UTF8_FIRSTBYTE(c) (((c) & 0xc0) != 0x80)

/* Get the next UTF-8 character, not advancing the pointer.
For instance, if eptr points to a 2-byte sequence of the form
(110xxxxx, 10xxxxxx), for example (110abcde, 10fghijk),
then c will be set to set to 00000abc defghijk */

#define GET_UTF8_CHAR(c, eptr) \
  c = *eptr; \
  if (c >= 0xc0) GETUTF8(c, eptr);

/* Base macro to pick up the remaining bytes of a UTF-8 character, not
advancing the pointer. */

#define GETUTF8(c, eptr) \
    { \
    if ((c & 0x20) == 0) \
      c = ((c & 0x1f) << 6) | (eptr[1] & 0x3f); \
    else if ((c & 0x10) == 0) \
      c = ((c & 0x0f) << 12) | ((eptr[1] & 0x3f) << 6) | (eptr[2] & 0x3f); \
    else if ((c & 0x08) == 0) \
      c = ((c & 0x07) << 18) | ((eptr[1] & 0x3f) << 12) | \
      ((eptr[2] & 0x3f) << 6) | (eptr[3] & 0x3f); \
    else if ((c & 0x04) == 0) \
      c = ((c & 0x03) << 24) | ((eptr[1] & 0x3f) << 18) | \
          ((eptr[2] & 0x3f) << 12) | ((eptr[3] & 0x3f) << 6) | \
          (eptr[4] & 0x3f); \
    else \
      c = ((c & 0x01) << 30) | ((eptr[1] & 0x3f) << 24) | \
          ((eptr[2] & 0x3f) << 18) | ((eptr[3] & 0x3f) << 12) | \
          ((eptr[4] & 0x3f) << 6) | (eptr[5] & 0x3f); \
    }


#ifdef __cplusplus
extern "C" {
#endif

/* Internal shared data tables.*/

extern const int            PRIV(utf8_table1)[];
extern const int            PRIV(utf8_table1_size);
extern const int            PRIV(utf8_table2)[];
extern const int            PRIV(utf8_table3)[]; /* Not used in VfmdByteArray */
extern const uint8_t        PRIV(utf8_table4)[];

/* Unicode character database (UCD) */

typedef struct {
  uint8_t script;     /* ucp_Arabic, etc. */
  uint8_t chartype;   /* ucp_Cc, etc. (general categories) */
  uint8_t gbprop;     /* ucp_gbControl, etc. (grapheme break property) */
  uint8_t caseset;    /* offset to multichar other cases or zero */
  int32_t other_case; /* offset to other case, or zero if none */
} ucd_record;

extern const uint32_t PRIV(ucd_caseless_sets)[];
extern const ucd_record  PRIV(ucd_records)[];
extern const uint8_t  PRIV(ucd_stage1)[];
extern const uint16_t PRIV(ucd_stage2)[];
extern const uint32_t PRIV(ucp_gentype)[];
extern const uint32_t PRIV(ucp_gbtable)[]; /* Not used in VfmdByteArray */

#ifdef __cplusplus
}
#endif

/* UCD access macros */

#define UCD_BLOCK_SIZE 128
#define GET_UCD(ch) (PRIV(ucd_records) + \
        PRIV(ucd_stage2)[PRIV(ucd_stage1)[(int)(ch) / UCD_BLOCK_SIZE] * \
        UCD_BLOCK_SIZE + (int)(ch) % UCD_BLOCK_SIZE])

#define UCD_CHARTYPE(ch)    GET_UCD(ch)->chartype
#define UCD_SCRIPT(ch)      GET_UCD(ch)->script
#define UCD_CATEGORY(ch)    PRIV(ucp_gentype)[UCD_CHARTYPE(ch)]
#define UCD_GRAPHBREAK(ch)  GET_UCD(ch)->gbprop
#define UCD_CASESET(ch)     GET_UCD(ch)->caseset
#define UCD_OTHERCASE(ch)   ((uint32_t)((int)ch + (int)(GET_UCD(ch)->other_case)))

// End of code from pcre_internal.h

// Following code adapted from pcre_ord2utf8.c

void VfmdByteArray::appendCharAsUTF8(int32_t codePointValue)
{
    if (codePointValue < 0) {
        return;
    }
    uint32_t cvalue = (uint32_t) codePointValue;
    int i, j;
    for (i = 0; i < PRIV(utf8_table1_size); i++)
        if ((int)cvalue <= PRIV(utf8_table1)[i]) break;
    assert(i <= 6);
    unsigned int additionalBytes = i + 1;
    copyOnWrite(additionalBytes);
    assert((m_offset + m_length) == d->size);
    reserveAdditionalBytes(additionalBytes);
    unsigned char *ucdata = reinterpret_cast<unsigned char *>(d->data) + d->size + additionalBytes - 1;
    for (j = i; j > 0; j--) {
        *ucdata-- = 0x80 | (cvalue & 0x3f);
        cvalue >>= 6;
    }
    *ucdata = PRIV(utf8_table2)[i] | cvalue;
    d->size += additionalBytes;
    m_length += additionalBytes;
}

// End of code adapted from pcre_ord2utf8.c

bool VfmdByteArray::isUTF8CharStartingAt(unsigned int byteIndex) const
{
    unsigned char c = byteAt(byteIndex);
    bool ok = IS_UTF8_FIRSTBYTE((unsigned char) c);
    return ok;
}

int VfmdByteArray::numberOfBytesInUTF8CharStartingAt(unsigned int byteIndex) const
{
    unsigned char c = (unsigned char) byteAt(byteIndex);
    bool isUtf8FirstByte = IS_UTF8_FIRSTBYTE((unsigned char) c);
    assert(isUtf8FirstByte);
    if (!isUtf8FirstByte) {
        return -1;
    }
    if (HAS_ADDITIONAL_UTF8_BYTES(c)) {
        unsigned char ab = NUMBER_OF_ADDITIONAL_UTF8_BYTES(c);
        int numOfBytes = ((int) ab) + 1;
        assert((byteIndex + numOfBytes) <= size());
        if ((byteIndex + numOfBytes) > size()) {
            return -1;
        }
        return numOfBytes;
    }
    return 1;
}

int VfmdByteArray::previousUTF8CharStartsAt(unsigned int byteIndex) const
{
    if (byteIndex > 0) {
        for (int i = byteIndex - 1; i >= 0; i--) {
            if (IS_UTF8_FIRSTBYTE(byteAt(i))) {
                return i;
            }
        }
    }
    return -1;
}

int32_t VfmdByteArray::codePointValueOfUTF8CharStartingAt(unsigned int byteIndex) const
{
    int n = numberOfBytesInUTF8CharStartingAt(byteIndex);
    assert(n >= 0);
    if (n < 0) {
        return -1;
    }
    uint32_t c;
    unsigned const char *ptr = reinterpret_cast<unsigned const char *>(data()) + byteIndex;
    GET_UTF8_CHAR(c, ptr);
    return (int32_t) c;
}

VfmdUnicodeProperties::GeneralCategory VfmdByteArray::categoryOfUTF8CharStartingAt(unsigned int byteIndex) const
{
    int n = numberOfBytesInUTF8CharStartingAt(byteIndex);
    assert(n >= 0);
    if (n < 0) {
        return VfmdUnicodeProperties::ucp_UndefinedGeneralCategory;
    }
    uint32_t c;
    unsigned const char *ptr = reinterpret_cast<unsigned const char *>(data()) + byteIndex;
    GET_UTF8_CHAR(c, ptr);
    return (VfmdUnicodeProperties::GeneralCategory) UCD_CHARTYPE(c);
}

VfmdUnicodeProperties::GeneralCategoryMajorClass VfmdByteArray::majorClassOfUTF8CharStartingAt(unsigned int byteIndex) const
{
    int n = numberOfBytesInUTF8CharStartingAt(byteIndex);
    assert(n >= 0);
    if (n < 0) {
        return VfmdUnicodeProperties::ucp_UndefinedGeneralCategoryMajorClass;
    }
    uint32_t c;
    unsigned const char *ptr = reinterpret_cast<unsigned const char *>(data()) + byteIndex;
    GET_UTF8_CHAR(c, ptr);
    return (VfmdUnicodeProperties::GeneralCategoryMajorClass) UCD_CATEGORY(c);
}

VfmdByteArray VfmdByteArray::caseFlipCodePointsOfCategory(VfmdUnicodeProperties::GeneralCategory category) const
{
    // Locate the first character that matches the given category
    int indexOfFirstByteToBeCaseFlipped = -1;
    unsigned int i = 0;
    unsigned int sz = size();
    while (i < sz) {
        int n = numberOfBytesInUTF8CharStartingAt(i);
        assert(n >= 0);
        if (n < 0) {
            return VfmdByteArray();
        }
        uint32_t c;
        unsigned const char *ptr = reinterpret_cast<unsigned const char *>(data()) + i;
        GET_UTF8_CHAR(c, ptr);
        const ucd_record *ucd = GET_UCD(c);
        VfmdUnicodeProperties::GeneralCategory gc = (VfmdUnicodeProperties::GeneralCategory) ucd->chartype;
        if (gc == category) {
            indexOfFirstByteToBeCaseFlipped = i;
            break;
        }
        i += n;
    }

    // If no character of the given category is found, return an implicit copy of this instance
    if (indexOfFirstByteToBeCaseFlipped < 0) {
        // No chars in this bytearray of the given category
        VfmdByteArray implicitCopy = *(this);
        return implicitCopy;
    }

    // Create the to-be-returned bytearray called caseFlippedBa.
    // In most cases, the case-flipped string will use the same number of bytes as the original.
    // If it takes more space, the additional bytes will have to be allocated on the fly.
    VfmdByteArray caseFlippedBa;
    caseFlippedBa.reserve(sz);

    // Copy initial bytes that don't need to be flipped
    assert(indexOfFirstByteToBeCaseFlipped >= 0);
    if (indexOfFirstByteToBeCaseFlipped > 0) {
        caseFlippedBa.append(data(), indexOfFirstByteToBeCaseFlipped);
    }

    // Add the rest of the characters to caseFlippedBa
    i = indexOfFirstByteToBeCaseFlipped;
    while (i < sz) {
        int n = numberOfBytesInUTF8CharStartingAt(i);
        assert(n >= 0);
        if (n < 0) {
            return VfmdByteArray();
        }
        uint32_t c;
        unsigned const char *ptr = reinterpret_cast<unsigned const char *>(data()) + i;
        GET_UTF8_CHAR(c, ptr);
        const ucd_record *ucd = GET_UCD(c);
        VfmdUnicodeProperties::GeneralCategory gc = (VfmdUnicodeProperties::GeneralCategory) ucd->chartype;
        if (gc == category) {
            // Flip case and append the case-flipped code point
            uint32_t caseFlippedCodePoint = ((uint32_t)((int)c + (int)(ucd->other_case)));
            caseFlippedBa.appendCharAsUTF8(caseFlippedCodePoint);
        } else {
            // Copy the bytes intact
            for (unsigned int j = 0; j < n; j++) {
                caseFlippedBa.appendByte(byteAt(i + j));
            }
        }
        i += n;
    }
    return caseFlippedBa;
}

VfmdByteArray VfmdByteArray::toUpperCase() const
{
    return caseFlipCodePointsOfCategory(VfmdUnicodeProperties::ucp_Ll);
}

VfmdByteArray VfmdByteArray::toLowerCase() const
{
    return caseFlipCodePointsOfCategory(VfmdUnicodeProperties::ucp_Lu);
}
