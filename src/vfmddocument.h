class VfmdElementRegistry;
class VfmdPreprocessor;
class VfmdInputLineSequence;

class VfmdDocument {
public:
    VfmdDocument(const VfmdElementRegistry *registry);
    ~VfmdDocument();

    bool addBytes(const char *data, int length);
    void end();

private:
    VfmdPreprocessor *m_preprocessor;
    VfmdInputLineSequence *m_documentLineSequence;
};
