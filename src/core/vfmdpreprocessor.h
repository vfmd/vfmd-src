#ifndef VFMDPREPROCESSOR_H
#define VFMDPREPROCESSOR_H

#include <stdlib.h>
#include "vfmdline.h"

class VfmdInputLineSequence;

class VfmdPreprocessor {
public:
    VfmdPreprocessor(VfmdInputLineSequence *lineSequence);
    ~VfmdPreprocessor();

    // Call addBytes() any number of times, and then finally
    // call end() at the end. addBytes() should be passed
    // at least 8 bytes per call, unless there are no more
    // bytes to add.
    bool addBytes(const char *data, int length);
    void end();

    static void preprocessByteArray(const char *data, unsigned int length, VfmdInputLineSequence *lineSequence);

private:
    VfmdByteArray m_text;
    VfmdByteArray m_unconsumedBytes;
    VfmdInputLineSequence *m_lineSequence;
    bool m_notEnoughBytesInLastAddBytesCall;
};

#endif // VFMDPREPROCESSOR_H
