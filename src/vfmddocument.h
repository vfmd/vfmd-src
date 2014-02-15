class VfmdElementRegistry;
class VfmdPreprocessor;
class VfmdInputLineSequence;
class VfmdElementTreeNode;

class VfmdDocument {
public:
    VfmdDocument(const VfmdElementRegistry *registry);
    ~VfmdDocument();

    bool addBytes(const char *data, int length);
    VfmdElementTreeNode* end();

private:
    /* Prevent copying of this class */
    VfmdDocument(const VfmdDocument& other);
    VfmdDocument& operator=(const VfmdDocument& other);

    VfmdPreprocessor *m_preprocessor;
    VfmdInputLineSequence *m_documentLineSequence;
};
