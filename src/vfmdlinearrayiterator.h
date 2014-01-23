#ifndef VFMDLINEARRAYITERATOR_H
#define VFMDLINEARRAYITERATOR_H

#include "vfmdregexp.h"

class VfmdLineArray;
class VfmdByteArray;

// VfmdLineArrayIterator provides a way of iterating over a VfmdLineArray as either
// lines or bytes as required.

// Instances of VfmdLineArrayIterator should be created using
// VfmdLineArray::begin() or VfmdLineArray::end()

class VfmdLineArrayIterator
{
public:
    /* Returns a newly allocated copy of this iterator */
    VfmdLineArrayIterator *copy() const;

    ~VfmdLineArrayIterator();

    /* Returns the immediate next byte (the iterator's position is always between bytes) */
    char nextByte() const;

    /* Returns the immediate previous byte (the iterator's position is always between bytes) */
    char previousByte() const;

    /* Is the nextByte the first byte of an escaped character? */
    bool isNextByteEscaped() const;

    /* Querying byte count */
    int numberOfBytesTillEndOfLine() const;
    int numberOfBytesTill(const VfmdLineArrayIterator *other) const;

    /* Querying bytes */
    VfmdByteArray bytesTillEndOfLine() const;
    VfmdByteArray bytesTill(const VfmdLineArrayIterator *other) const;

    /* Moving the iterator across 'n' number of bytes */
    void moveForward(unsigned int n);
    void moveBackward(unsigned int n);

    /* Advancing the iterator */
    void moveForwardTillEndOfLine();
    bool moveForwardOverBytesInString(const char *str);
    bool moveForwardOverBytesNotInString(const char *str);
    bool moveForwardOverRegexp(const VfmdRegexp &regexp);

    /* Moving the iterator to another iterator's position */
    bool moveTo(const VfmdLineArrayIterator *other);

    /* Is this iterator at the beginning / end of the bytestream? */
    bool isAtBeginning() const;
    bool isAtEnd() const;
    bool isAtLastLine() const;

    /* Compare with other iterator objects */
    bool isEqualTo(const VfmdLineArrayIterator *other) const;
    bool isBefore(const VfmdLineArrayIterator *other) const;
    bool isAfter(const VfmdLineArrayIterator *other) const;
    bool operator ==(const VfmdLineArrayIterator &other) const;
    bool operator <(const VfmdLineArrayIterator &other) const;
    bool operator >(const VfmdLineArrayIterator &other) const;

private:
    VfmdLineArrayIterator(const VfmdLineArray *lineArray, unsigned int lineIndex, unsigned int byteIndex, bool isNextByteEscaped = false);

    void moveTo(unsigned int lineIndex, unsigned int byteIndex);
    void foreachLineSegmentsTill(const VfmdLineArrayIterator *other, void *ctx, bool (*fn)(void *ctx, const VfmdByteArray &ba)) const;

    struct ForeachLineSegmentsStringMatchingContext {
        ForeachLineSegmentsStringMatchingContext(unsigned int li, unsigned int bi, const char *s)
            : lineIndex(li), byteIndex(bi), str(s) { }
        unsigned int lineIndex;
        unsigned int byteIndex;
        const char *str;
    };

    const VfmdLineArray *m_lineArray;
    unsigned int m_lineIndex;
    unsigned int m_byteIndex;
    bool m_isNextByteEscaped;

    friend class VfmdLineArray;
    friend bool advanceOverMatchingBytes(void *ctx, const VfmdByteArray &ba);
    friend bool advanceOverNonMatchingBytes(void *ctx, const VfmdByteArray &ba);
};

#endif // VFMDLINEARRAYITERATOR_H
