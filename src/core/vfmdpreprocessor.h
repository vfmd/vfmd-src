#ifndef VFMDPREPROCESSOR_H
#define VFMDPREPROCESSOR_H

#include <stdlib.h>
#include "vfmdline.h"

class VfmdInputLineSequence;

class VfmdPreprocessor {
public:
    VfmdPreprocessor(VfmdInputLineSequence *lineSequence);
    ~VfmdPreprocessor();

    void addBytes(const char *data, int length);
    void end();

private:
    void ensureBufferAllocated();

    VfmdLine m_unconsumedBytes;
    VfmdInputLineSequence *m_lineSequence;
};

#endif // VFMDPREPROCESSOR_H
