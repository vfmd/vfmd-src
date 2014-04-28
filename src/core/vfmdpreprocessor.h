#ifndef VFMDPREPROCESSOR_H
#define VFMDPREPROCESSOR_H

#include <stdlib.h>
#include "vfmdline.h"

class VfmdInputLineSequence;

class VfmdPreprocessor {
public:
    VfmdPreprocessor(VfmdInputLineSequence *lineSequence);
    ~VfmdPreprocessor();

    void addBytes(const char *data, int length); // minimum 3 bytes per call
    void end();

    static void preprocessByteArray(const char *data, unsigned int length, VfmdInputLineSequence *lineSequence);

private:
    VfmdByteArray m_text;
    VfmdByteArray m_unconsumedBytes;
    VfmdInputLineSequence *m_lineSequence;
};

#endif // VFMDPREPROCESSOR_H
