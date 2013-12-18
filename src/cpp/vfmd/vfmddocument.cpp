#include <stdio.h>
#include <stdlib.h>
#include "VfmdDocument.h"

VfmdDocument::VfmdDocument()
    : m_bufferSize(4096),
      m_buffer(NULL),
      m_isBufferAllocated(false)
{
}

VfmdDocument::~VfmdDocument()
{
    free(m_buffer);
}

void VfmdDocument::ensureBufferAllocated()
{
    if (!m_isBufferAllocated) {
        m_buffer = static_cast<char*>(malloc(m_bufferSize));
    }
}

bool VfmdDocument::setBufferSize(unsigned int size)
{
    if (!m_isBufferAllocated) {
        m_bufferSize = size;
        return true;
    }
    return false;
}

unsigned int VfmdDocument::bufferSize() const
{
    return m_bufferSize;
}

bool VfmdDocument::addBytes(const char *data, int length)
{
    printf("TODO: addBytes\n");
}

