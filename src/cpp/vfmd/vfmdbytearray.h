#ifndef VFMDBYTEARRAY_H
#define VFMDBYTEARRAY_H

#include <stdlib.h>

// VfmdByteArray
// Stores a byte array. Uses implicit data sharing.

class VfmdByteArray
{
public:

    /* Create an empty byte array */
    VfmdByteArray();

    /* Create a byte array with a copy of 'length' bytes starting from 'data' */
    VfmdByteArray(const char *data, int length);

    /* Destructor */
    ~VfmdByteArray();

    /* Append a copy of 'length' bytes starting from 'data' */
    void append(const char *data, int length);

    /* Append one or more bytes */
    void appendByte(char byte1);
    void appendBytes(char byte1, char byte2);
    void appendBytes(char byte1, char byte2, char byte3);
    void appendBytes(char byte1, char byte2, char byte3, char byte4);

    /* Remove the left 'count' bytes (without any data-replication) */
    bool chopLeft(unsigned int count);

    /* Remove the right 'count' bytes (without any data-replication) */
    bool chopRight(unsigned int count);

    /* Get the data and the length of the data */
    const char *data() const;
    size_t size() const;

    /* Ensure there are atleast 'length' bytes of allocated space.
     * This can cause an internal realloc and/or data copy. */
    void reserve(size_t length);

    /* Find out what the reserved space is. */
    size_t capacity() const;

    /* Empty the bytearray */
    void clear();

    /* Copy this object */
    VfmdByteArray(const VfmdByteArray &other);
    VfmdByteArray& operator=(const VfmdByteArray& other);

    /* Deep-copy this object */
    VfmdByteArray clone() const;

    /* Minimize the memory footprint of this object.
     * This can cause an internal realloc and/or data copy. */
    void squeeze();

    /* Get a copy of the data as a null-terminated string */
    const char *c_str() const;

private:
    void ref();
    void deref();
    void copyOnWrite(size_t additionalSpaceRequired = 0);

    class Private;
    Private *d;

    size_t m_leftOffset;
    size_t m_rightOffset;
};

#endif // VFMDBYTEARRAY_H