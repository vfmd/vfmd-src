#include <string.h>
#include "vfmdoutputdevice.h"

// VfmdOutputDevice

VfmdOutputDevice::VfmdOutputDevice()
{
}

VfmdOutputDevice::~VfmdOutputDevice()
{
}

bool VfmdOutputDevice::write(const char byte)
{
    return writeData(&byte, 1);
}

bool VfmdOutputDevice::write(const char *data, int len)
{
    if (len > 0) {
        return writeData(data, len);
    } else {
        return writeData(data, strlen(data));
    }
}

bool VfmdOutputDevice::write(const VfmdByteArray &ba)
{
    return writeData(ba.data(), ba.size());
}

// VfmdFileOutputDevice

VfmdFileOutputDevice::VfmdFileOutputDevice(const char *fileName)
    : m_file(fopen(fileName, "w"))
{
}

VfmdFileOutputDevice::VfmdFileOutputDevice(FILE *file)
    : m_file(file)
{
}

VfmdFileOutputDevice::~VfmdFileOutputDevice()
{
    close();
}

bool VfmdFileOutputDevice::isValid() const
{
    return (m_file != 0);
}

void VfmdFileOutputDevice::close()
{
    if (m_file != stdout) {
        fclose(m_file);
        m_file = 0;
    }
}

bool VfmdFileOutputDevice::writeData(const char *data, int len)
{
    // Assumes there are no null-bytes in 'data'
    if (m_file) {
        fprintf(m_file, "%.*s", len, data);
        return true;
    }
    return false;
}

// VfmdConsoleOutputDevice

VfmdConsoleOutputDevice::VfmdConsoleOutputDevice()
    : VfmdFileOutputDevice(stdout)
{
}

VfmdConsoleOutputDevice::~VfmdConsoleOutputDevice()
{
}

// VfmdBufferOutputDevice

VfmdBufferOutputDevice::VfmdBufferOutputDevice()
{
}

VfmdBufferOutputDevice::~VfmdBufferOutputDevice()
{
}

int VfmdBufferOutputDevice::bytesWritten() const
{
    return m_buffer.size();
}

const VfmdByteArray& VfmdBufferOutputDevice::buffer() const
{
    return m_buffer;
}

void VfmdBufferOutputDevice::squeezeBuffer()
{
    m_buffer.squeeze();
}

bool VfmdBufferOutputDevice::writeData(const char *data, int len)
{
    m_buffer.append(data, len);
    return true;
}
