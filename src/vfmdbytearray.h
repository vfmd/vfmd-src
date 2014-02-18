#ifndef VFMDBYTEARRAY_H
#define VFMDBYTEARRAY_H

#include <stdlib.h>
#include <stdint.h>
#include "vfmdunicodeproperties.h"

// VfmdByteArray
// Stores an array of bytes. Uses implicit data sharing.
// Can also be used to store Unicode strings encoded as UTF-8.

class VfmdByteArray
{
public:

    /* Create an empty and invalid byte array */
    VfmdByteArray();

    /* Create a byte array with a copy of the null-terminated string 'str' */
    VfmdByteArray(const char *str);

    /* Create a byte array with a copy of 'length' bytes starting from 'data' */
    VfmdByteArray(const char *data, int length);

    /* Destructor */
    ~VfmdByteArray();

    /* Uninitialized bytearrays are invalid */
    bool isValid() const;

    /* Append a copy of 'length' bytes starting from 'data' */
    void append(const char *data, int length);

    /* Append a copy of 'other' */
    void append(const VfmdByteArray &other);

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

    /* Utility query functions */
    char byteAt(unsigned int pos) const;
    char lastByte() const;
    bool startsWith(const char *str) const;
    bool startsWith(const VfmdByteArray &ba) const;
    char firstNonSpace() const;
    char lastNonSpace() const;
    int indexOfFirstNonSpace() const;
    int indexOfLastNonSpace() const;

    /* Return the left 'count' bytes as a bytearray (without any data-replication) */
    VfmdByteArray left(unsigned int count) const;

    /* Return the right 'count' bytes as a bytearray (without any data-replication) */
    VfmdByteArray right(unsigned int count) const;

    /* Return the bytes starting from the 'n'th position till the end as a bytearray (without any data-replication) */
    VfmdByteArray mid(unsigned int n) const;

    /* Return the 'l' bytes starting from the 'n'th position as a bytearray (without any data-replication) */
    VfmdByteArray mid(unsigned int n, unsigned int l) const;

    /* Trimming (remove whitespace at beginning or end) */
    void trimLeft();
    void trimRight();
    VfmdByteArray leftTrimmed() const;
    VfmdByteArray rightTrimmed() const;
    VfmdByteArray trimmed() const;

    /* Simplifying (trim, and replace internal whitespace with a single space) */
    VfmdByteArray simplified() const;

    /* Removing bytes */
    VfmdByteArray bytesInStringRemoved(const char *bytesToRemove) const;

    /* Chomping (removing trailing newline) */
    void chomp();
    VfmdByteArray chomped() const;

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
    VfmdByteArray *copy() const;

    /* Deep-copy this object */
    VfmdByteArray clone() const;

    /* Minimize the memory footprint of this object.
     * This can cause an internal realloc and/or data copy. */
    void squeeze();

    /* Compare with others */
    bool isEqualTo(const char *str, int len = 0) const;
    bool operator==(const VfmdByteArray &other) const;

    /* Get a copy of the data as a null-terminated string */
    const char *c_str() const;

    /* Print the contained data */
    void print() const;

    // Methods that offer Unicode-related functionality.
    // These methods assume that the VfmdByteArray instance contains UTF-8 data

    /* Add a Unicode Code Point to the bytearray. The bytearray will include it
     * after encoding it as UTF-8. */
    void appendCharAsUTF8(int32_t codePointValue);

    /* Does byteIndex form the starting byte of a UTF-8 code point */
    bool isUTF8CharStartingAt(unsigned int byteIndex) const;

    /* How many bytes are in the UTF-8 code point starting at byteIndex? */
    int numberOfBytesInUTF8CharStartingAt(unsigned int byteIndex) const;

    /* Assuming that byteIndex forms the starting byte of a UTF-8 code point,
     * which is the byteIndex at which the previous code point starts? */
    int previousUTF8CharStartsAt(unsigned int byteIndex) const;

    /* Assuming that byteIndex forms the starting byte of a UTF-8 code point,
     * what code point starts at byteIndex? */
    int32_t codePointValueOfUTF8CharStartingAt(unsigned int byteIndex) const;

    /* What is the General_Category of the UTF-8 code point
     * whose starting byte is at byteIndex? */
    VfmdUnicodeProperties::GeneralCategory categoryOfUTF8CharStartingAt(unsigned int byteIndex) const;

    /* What is the major class of the General_Category of the UTF-8 code point
     * whose starting byte is at byteIndex? */
    VfmdUnicodeProperties::GeneralCategoryMajorClass majorClassOfUTF8CharStartingAt(unsigned int byteIndex) const;

    /* Returns a bytearray with the uppercase equivalent of this bytearray, assuming
     * that this bytearray is in UTF-8 encoding. The returned bytearray is in UTF-8. */
    VfmdByteArray toUpperCase() const;

    /* Returns a bytearray with the lowercase equivalent of this bytearray, assuming
     * that this bytearray is in UTF-8 encoding. The returned bytearray is in UTF-8. */
    VfmdByteArray toLowerCase() const;

private:
    VfmdByteArray caseFlipCodePointsOfCategory(VfmdUnicodeProperties::GeneralCategory category) const;

    void ref();
    void deref();
    void copyOnWrite(size_t additionalSpaceRequired = 0);

    class Private;
    Private *d;

    size_t m_leftOffset;
    size_t m_rightOffset;
};

#endif // VFMDBYTEARRAY_H
