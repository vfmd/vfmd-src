

class VfmdPreprocessor {
public:
    typedef void (*LineCallbackFunc) (void *context, const char *data, int length);

    VfmdPreprocessor();
    ~VfmdPreprocessor();

    bool setBufferSize(unsigned int bufferSize);
    unsigned int bufferSize() const;

    int addBytes(char *data, int length);
    void setLineCallback(LineCallbackFunc fn);
    void setLineCallbackContext(void *context);

private:
    void ensureBufferAllocated();

    char *m_buffer;
    unsigned int m_bufferSize;
    bool m_isBufferAllocated;
    unsigned int m_filledBytes;

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
};

