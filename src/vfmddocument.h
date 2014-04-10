class VfmdElementRegistry;
class VfmdPreprocessor;
class VfmdInputLineSequence;
class VfmdElementTreeNode;

#include "vfmdbytearray.h"

class VfmdDocument {
public:
    VfmdDocument(const VfmdElementRegistry *registry);
    ~VfmdDocument();

    void addBytes(const char *data, int length);
    VfmdElementTreeNode* end();

    static VfmdElementTreeNode* parseByteArray(const VfmdByteArray &text, const VfmdElementRegistry *registry);

private:
    /* Prevent copying of this class */
    VfmdDocument(const VfmdDocument& other);
    VfmdDocument& operator=(const VfmdDocument& other);

    VfmdPreprocessor *m_preprocessor;
    VfmdInputLineSequence *m_documentLineSequence;
};
