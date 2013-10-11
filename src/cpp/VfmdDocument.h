
class VfmdDocument {
public:
    VfmdDocument();
    ~VfmdDocument();

    bool setBufferSize(unsigned int size);
    unsigned int bufferSize() const;

    bool addBytes(const char *data, int length);

    // ParseTree *done();

private:
    void ensureBufferAllocated();

    unsigned int m_bufferSize;
    char *m_buffer;
    bool m_isBufferAllocated;
};
