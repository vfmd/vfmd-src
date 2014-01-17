#include <assert.h>
#include <string.h>
#include "vfmdlinearray.h"
#include "vfmdline.h"
#include "vfmdlinearrayiterator.h"

#define ALLOC_CHUNK_SIZE (128)

VfmdLineArray::VfmdLineArray()
{
    m_lines = new VfmdPointerArray<VfmdLine>(ALLOC_CHUNK_SIZE);
}

VfmdLineArray::~VfmdLineArray()
{
    for (unsigned int i = 0; i < m_lines->size(); i++) {
        delete m_lines->itemAt(i);
    }
    delete m_lines;
}

void VfmdLineArray::addLine(const VfmdLine &line)
{
    if (line.isValid()) {
        m_lines->append(new VfmdLine(line)); // append an implicit copy of the line
    }
}

unsigned int VfmdLineArray::lineCount() const
{
    return m_lines->size();
}

const VfmdLine *VfmdLineArray::lineAt(unsigned int lineIndex) const
{
    return m_lines->itemAt(lineIndex);
}

void VfmdLineArray::print() const
{
    for (unsigned int i = 0; i < m_lines->size(); i++) {
        m_lines->itemAt(i)->print();
    }
}

VfmdLineArrayIterator* VfmdLineArray::begin() const
{
    return new VfmdLineArrayIterator(this, 0, 0);
}

VfmdLineArrayIterator* VfmdLineArray::end() const
{
    return new VfmdLineArrayIterator(this, lineCount(), 0);
}
