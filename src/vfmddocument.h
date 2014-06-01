#include "vfmdconstants.h"
#include "vfmdoutputdevice.h"

class VfmdElementRegistry;
class VfmdInputLineSequence;
class VfmdPreprocessor;
class VfmdLinkRefMap;
class VfmdElementTreeNode;

class VfmdDocument {

public:
    VfmdDocument();
    ~VfmdDocument();

    // If you have the complete document content available:
    // Use setContent()
    void setContent(const char *data, int length);

    // If you get the document content in chunks:
    // Use one or more addPartialContent() calls followed
    // by an endOfContent() call. The length of each partial
    // content should be at least 8 bytes, unless there is
    // no more content to add.
    // After a setContent() or an endOfContent() call,
    // addPartialContent() has no effect.
    // This method returns true if the partial content was
    // added successfully and false if not.
    bool addPartialContent(const char *data, int length);
    void endOfContent();

    // After a setContent() or an endOfContent() call,
    // you can render the parsed document.
    void render(VfmdConstants::RenderFormat format, int renderOptions,
                VfmdOutputDevice *outputDevice) const;

    // You can obtain and modify the syntax registry before
    // adding content to influence how the content shall be parsed.
    VfmdElementRegistry *syntaxRegistry() const;

    // After a setContent() or an endOfContent() call,
    // you can obtain a parse tree of the document.
    VfmdElementTreeNode *parseTree() const;

private:
    /* Prevent copying of this class */
    VfmdDocument(const VfmdDocument& other);
    VfmdDocument& operator=(const VfmdDocument& other);

    bool m_isReadingPartialContent;
    bool m_isEndOfContent;
    VfmdLinkRefMap *m_linkRefMap;
    VfmdElementRegistry *m_registry;
    VfmdPreprocessor *m_preprocessor;
    VfmdInputLineSequence *m_documentLineSequence;
    VfmdElementTreeNode *m_parseTree;
};
