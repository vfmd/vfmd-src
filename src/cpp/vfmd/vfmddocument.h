class VfmdPreprocessor;
class VfmdInputLineSequence;

class VfmdDocument {
public:
    VfmdDocument();
    ~VfmdDocument();

    bool addBytes(const char *data, int length);
    void end();

private:
    VfmdPreprocessor *m_preprocessor;
    VfmdInputLineSequence *m_documentLineSequence;
};
