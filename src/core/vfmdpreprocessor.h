#ifndef VFMDPREPROCESSOR_H
#define VFMDPREPROCESSOR_H

#include <stdlib.h>
#include "vfmdline.h"

class VfmdPreprocessor {
public:
    typedef void (*LineCallbackFunc) (void *context, const VfmdLine &line);

    VfmdPreprocessor();
    ~VfmdPreprocessor();

    void addBytes(const char *data, int length);
    void end();

    void setLineCallback(LineCallbackFunc fn);
    void setLineCallbackContext(void *context);

private:
    void ensureBufferAllocated();

    VfmdByteArray m_unconsumedBytes;
    LineCallbackFunc m_lineCallback;
    void *m_lineCallbackContext;
};

#endif // VFMDPREPROCESSOR_H
