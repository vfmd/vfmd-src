#ifndef VFMDPREPROCESSOR_H
#define VFMDPREPROCESSOR_H

#include <stdlib.h>
#include "vfmdbytearray.h"

class VfmdPreprocessor {
public:
    typedef void (*LineCallbackFunc) (void *context, const VfmdByteArray &line);

    VfmdPreprocessor();
    ~VfmdPreprocessor();

    int addBytes(char *data, int length);
    void end();

    void setLineCallback(LineCallbackFunc fn);
    void setLineCallbackContext(void *context);

private:
    void ensureBufferAllocated();

    VfmdByteArray m_line;
    LineCallbackFunc m_lineCallback;
    void *m_lineCallbackContext;

    struct UnfinishedCodePoint {
        UnfinishedCodePoint() : c(0), d(0), e(0), bytesSeen(0), bytesRemaining(0) { }
        void set(unsigned char _c, unsigned char _d, unsigned char _e,
                 int _bytesSeen, int _bytesRemaining) {
            c = _c; d = _d; e = _e;
            bytesSeen = _bytesSeen; bytesRemaining = _bytesRemaining;
        }
        unsigned char c, d, e;
        int bytesSeen;
        int bytesRemaining;
    } m_unfinishedCodePoint; // Used when the data of addBytes contains a partial code point

    bool m_isUnfinishedCRLF;
    int64_t m_codePointCount; // number of code points in a line
};

#endif // VFMDPREPROCESSOR_H
