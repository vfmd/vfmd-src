

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
};

