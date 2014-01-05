#include "vfmdbytearray.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define REALLOC_CHUNK_SIZE (1024)

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

    void append(const char *str, size_t len) {
        size_t requiredSize = (size + len);
        if (requiredSize > allocatedSize) {
            // Better to allocate in chunks
            if (requiredSize < REALLOC_CHUNK_SIZE) {
                requiredSize = REALLOC_CHUNK_SIZE;
            } else {
                requiredSize += REALLOC_CHUNK_SIZE - (requiredSize % REALLOC_CHUNK_SIZE);
            }
            reallocateBuffer(requiredSize);
        }
        memcpy(data + size, str, len);
        size += len;
    }

    void appendByte(char byte) {
        size_t requiredSize = (size + 1);
        if (requiredSize > allocatedSize) {
            // Better to allocate in chunks
            if (requiredSize < REALLOC_CHUNK_SIZE) {
                requiredSize = REALLOC_CHUNK_SIZE;
            } else {
                requiredSize += REALLOC_CHUNK_SIZE - (requiredSize % REALLOC_CHUNK_SIZE);
            }
            reallocateBuffer(requiredSize);
        }
        data[size] = byte;
        size++;
    }

    unsigned int refCount;

    char *data;
    size_t size;
    size_t allocatedSize;
};

VfmdByteArray::VfmdByteArray()
    : d(new Private()), m_leftOffset(0), m_rightOffset(0)
{
}

VfmdByteArray::VfmdByteArray(const char *str)
    : d(new Private(str, strlen(str))), m_leftOffset(0), m_rightOffset(0)
{
}

VfmdByteArray::VfmdByteArray(const char *data, int length)
    : d(new Private(data, length)), m_leftOffset(0), m_rightOffset(0)
{
}

VfmdByteArray::~VfmdByteArray()
{
    deref();
}

bool VfmdByteArray::isValid() const
{
    return (d->data != 0);
}

void VfmdByteArray::append(const char *data, int length)
{
    copyOnWrite(length);
    d->append(data, length);
}

void VfmdByteArray::appendByte(char byte1)
{
    copyOnWrite(1);
    d->appendByte(byte1);
}

void VfmdByteArray::appendBytes(char byte1, char byte2)
{
    copyOnWrite(2);
    d->appendByte(byte1);
    d->appendByte(byte2);
}

void VfmdByteArray::appendBytes(char byte1, char byte2, char byte3)
{
    copyOnWrite(3);
    d->appendByte(byte1);
    d->appendByte(byte2);
    d->appendByte(byte3);
}

void VfmdByteArray::appendBytes(char byte1, char byte2, char byte3, char byte4)
{
    copyOnWrite(4);
    d->appendByte(byte1);
    d->appendByte(byte2);
    d->appendByte(byte3);
    d->appendByte(byte4);
}

bool VfmdByteArray::chopLeft(unsigned int count)
{
    if (count > size()) {
        return false;
    }
    m_leftOffset += count;
    return true;
}

bool VfmdByteArray::chopRight(unsigned int count)
{
    if (count > size()) {
        return false;
    }
    if (d->refCount > 1) {
        // Some other instance is using the same data.
        // Let's not modify d.
        m_rightOffset += count;
    } else {
        // No other instance is using the same data.
        // We can modify d.
        d->size -= (count + m_rightOffset);
        m_rightOffset = 0;
    }
    return true;
}

const char* VfmdByteArray::data() const
{
    return (d->data + m_leftOffset);
}

size_t VfmdByteArray::size() const
{
    return (d->size - m_leftOffset - m_rightOffset);
}

bool VfmdByteArray::startsWith(const char *str) const
{
    const char *data_ptr = data();
    size_t sz = size();
    for (unsigned int i = 0; i < sz; i++) {
        const char c = str[i];
        if (c == 0) {
            return true;
        }
        if (data_ptr[i] != c) {
            return false;
        }
    }
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

void VfmdByteArray::reserve(size_t length)
{
    if (d->allocatedSize < length) {
        d->reallocateBuffer(length);
    }
}

size_t VfmdByteArray::capacity() const
{
    return d->allocatedSize;
}

void VfmdByteArray::clear()
{
    if (d->refCount > 1) {
        // Some other instance is using the same data.
        deref();
        d = new Private();
    } else {
        // No other instance is using the same data.
        d->size = 0;
    }
    m_leftOffset = 0;
    m_rightOffset = 0;
}

// Implicit sharing stuff follows

VfmdByteArray::VfmdByteArray(const VfmdByteArray &other)
    : d(other.d) {
    ref();
    m_leftOffset = other.m_leftOffset;
    m_rightOffset = other.m_rightOffset;
}

VfmdByteArray& VfmdByteArray::operator=(const VfmdByteArray &other) {
    if (this != &other) {
        deref(); // dereference existing data
        d = other.d;
        ref();   // reference new data
        m_leftOffset = other.m_leftOffset;
        m_rightOffset = other.m_rightOffset;
    }
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
        m_leftOffset = 0;
        m_rightOffset = 0;
    }
}

void VfmdByteArray::copyOnWrite(size_t additionalSpaceRequired) {
    if (d->refCount > 1) {
        deref(); // dereference existing data
        // copy data (refCount will be 1 for new copy)
        d = new Private(data(), size(), size() + additionalSpaceRequired);
        m_leftOffset = 0;
        m_rightOffset = 0;
    }
}

VfmdByteArray VfmdByteArray::clone() const
{
    return VfmdByteArray(data(), size());
}

void VfmdByteArray::squeeze()
{
    if (d->refCount > 1) {
        // Some other instance is using the same data.
        // Let's make a copy of the data, so that the other instance is not affected.
        deref();
        d = new Private(data(), size());
        m_leftOffset = 0;
        m_rightOffset = 0;
    } else {
        // No other instance is using the same data.
        // We can realloc the data we hold.
        assert(d->refCount == 1);
        if (m_leftOffset > 0) {
            memmove(d->data, d->data + m_leftOffset, d->size - m_leftOffset - m_rightOffset);
            d->size = d->size - m_leftOffset - m_rightOffset;
            m_leftOffset = 0;
            m_rightOffset = 0;
        }
        d->reallocateBuffer(size());
        d->size = d->size - m_rightOffset;
        m_rightOffset = 0;
    }
}

// Printing helper

const char* VfmdByteArray::c_str() const
{
    size_t len = size();
    char *str = static_cast<char *>(malloc(len + 1));
    memcpy(str, data(), len);
    str[len] = '\0';
    return str;
}

void VfmdByteArray::print(const char *prefix) const {
    if (prefix) {
        printf("%s: ", prefix);
    }
    const char *data_ptr = data();
    size_t sz = size();
    for (unsigned int i = 0; i < sz; i++) {
        printf("%c", data_ptr[i]);
    }
}
