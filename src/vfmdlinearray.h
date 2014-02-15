#ifndef VfmdLineArray_H
#define VfmdLineArray_H

#include "vfmdpointerarray.h"
#include "vfmdbytearray.h"

class VfmdLine;
class VfmdElementRegistry;
class VfmdLineArrayIterator;

class VfmdLineArray
{
public:
    VfmdLineArray();
    ~VfmdLineArray();

    void addLine(const VfmdLine &line);

    // trimLeft(): Remove any leading whitespace characters in the
    // line array, even if they exist across multiple lines
    void trimLeft();

    // trimRight(): Remove any trailing whitespace characters in the
    // line array, even if they exist across multiple lines
    void trimRight();

    // trim(): Remove leading and trailing whitespace characters in the
    // line array, even if they exist across multiple lines
    void trim();

    unsigned int lineCount() const;
    const VfmdLine *lineAt(unsigned int lineIndex) const;

    void print() const;

    /* Traverse the line array using an iterator.
     * If you modify the line array, the iterator becomes invalid,
     * and you need to call begin() or end() afresh. */
    VfmdLineArrayIterator begin() const;
    VfmdLineArrayIterator end() const;

private:
    /* Prevent copying of this class */
    VfmdLineArray(const VfmdLineArray& other);
    VfmdLineArray& operator=(const VfmdLineArray& other);

    VfmdPointerArray<VfmdLine> *m_lines;
};

#endif // VfmdLineArray_H
