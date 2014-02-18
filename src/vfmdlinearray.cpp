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

void VfmdLineArray::trimLeft()
{
    while (m_lines->size() > 0) {
        VfmdLine *firstLine = m_lines->itemAt(0);
        firstLine->trimLeft();
        if (firstLine->size() > 0) {
            break;
        }
        m_lines->removeItemAt(0);
    }
}

void VfmdLineArray::trimRight()
{
    while (m_lines->size() > 0) {
        VfmdLine *lastLine = m_lines->lastItem();
        lastLine->trimRight();
        if (lastLine->size() > 0) {
            break;
        }
        m_lines->removeLastItem();
    }
}

void VfmdLineArray::trim()
{
    trimRight();
    trimLeft();
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

void VfmdLineArray::clear()
{
    for (unsigned int i = 0; i < m_lines->size(); i++) {
        delete m_lines->itemAt(i);
    }
    delete m_lines;
    m_lines = new VfmdPointerArray<VfmdLine>(ALLOC_CHUNK_SIZE);
}

VfmdLineArrayIterator VfmdLineArray::begin() const
{
    return VfmdLineArrayIterator(this, 0, 0);
}

VfmdLineArrayIterator VfmdLineArray::end() const
{
    return VfmdLineArrayIterator(this, lineCount(), 0);
}
