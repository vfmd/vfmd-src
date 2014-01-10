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

    unsigned int lineCount() const;
    const VfmdLine *lineAt(unsigned int lineIndex) const;

    void print() const;

    /* Iterators for going over the line array.
     * If you modify the line array, the iterators become invalid,
     * and you need to call begin() or end() afresh.
     * The caller is responsible for freeing the returned iterators. */
    VfmdLineArrayIterator *begin() const;
    VfmdLineArrayIterator *end() const;

private:
    VfmdPointerArray<VfmdLine> *m_lines;
};

#endif // VfmdLineArray_H
