#ifndef VFMDOUTPUTDEVICE_H
#define VFMDOUTPUTDEVICE_H

#include <stdio.h>
#include "vfmdbytearray.h"

class VfmdOutputDevice
{
public:
    VfmdOutputDevice();
    virtual ~VfmdOutputDevice();
    bool write(const char byte);
    bool write(const char *data, int len = 0);
    bool write(const VfmdByteArray &ba);

protected:
    virtual bool writeData(const char *data, int len) = 0;
};

class VfmdFileOutputDevice : public VfmdOutputDevice
{
public:
    VfmdFileOutputDevice(const char *fileName);
    VfmdFileOutputDevice(FILE *file);
    virtual ~VfmdFileOutputDevice();
    bool isValid() const;
    void close();

protected:
    virtual bool writeData(const char *data, int len);
    FILE *m_file;
};

class VfmdConsoleOutputDevice : public VfmdFileOutputDevice
{
public:
    VfmdConsoleOutputDevice();
    virtual ~VfmdConsoleOutputDevice();
};

class VfmdBufferOutputDevice : public VfmdOutputDevice
{
public:
    VfmdBufferOutputDevice();
    virtual ~VfmdBufferOutputDevice();
    int bytesWritten() const;
    const VfmdByteArray& buffer() const;
    void squeezeBuffer();

protected:
    virtual bool writeData(const char *data, int len);
    VfmdByteArray m_buffer;
};

#endif // VFMDOUTPUTDEVICE_H