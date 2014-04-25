#ifndef VFMDSPANELEMENTREGISTRY_H
#define VFMDSPANELEMENTREGISTRY_H

#include "core/vfmdpointerarray.h"
#include "vfmdbytearray.h"

class VfmdSpanElementHandler;
class VfmdBlockElementHandler;
class VfmdLinkRefMap;

template<class T> class RegistryData;

class VfmdElementRegistry
{
public:
    VfmdElementRegistry();
    ~VfmdElementRegistry();

    // Block elements

    /* Add a block element handler to the registry at the end.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler);

    /* Add a block element handler to be invoked before an existing block element handler.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool insertBlockElementBeforeExistingBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler, int existingTypeId);

    /* Returns the index of a block element given the typeId. Returns -1 on no match. */
    int indexOfBlockElement(int typeId) const;

    /* Check for existence of a block element */
    bool containsBlockElement(int typeId) const;

    /* Remove and free a block element handler in the registry */
    void removeBlockElement(int typeId);

    /* Blocks that can abut a paragraph */
    void setBlockCanAbutParagraph(int typeId, bool yes = true);
    VfmdPointerArray<VfmdBlockElementHandler> *blockHandlersThatCanAbutParagraph() const;

    /* Querying block elements */
    unsigned int blockElementsCount() const;
    VfmdBlockElementHandler *blockElementHandler(unsigned int index) const;

    // Span elements

    enum SpanElementOptions {
        /* By default, the handler is invoked when the current byte
         * is a trigger byte. Use this option to inform the registry
         * that this handler is to be invoked at or ahead of
         * the trigger byte. This is useful when the trigger byte does not
         * always occur at the start of the span element. If this option is
         * used, the span handler's 'identifySpanTagStartingBetween()' method
         * is invoked instead of the usual 'identifySpanTagStartingAt()' method. */
        TRIGGER_BEFORE_TRIGGER_BYTE = 1,

        /* By default, the handler is invoked only when the trigger byte
         * is not backslash-escaped. Use this option to inform the registry
         * that this handler is to be invoked on the given trigger bytes,
         * irrespective of whether the trigger byte is backslash-escaped or not. */
        TRIGGER_EVEN_IF_TRIGGER_BYTE_IS_ESCAPED = 2
    };

    /* Add a span element handler that should be invoked only when one of the triggerBytes
     * is encountered in the text. Atleast one triggerByte should be provided.
     * Multiple span element handlers can be registered for a particular triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                           const VfmdByteArray &triggerBytes, int spanElementOptions = 0);

    /* Same as 'appendSpanElement()', except that the handler is added such that
     * it shall get invoked before any previously registered handlers. */
    bool prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                            const VfmdByteArray &triggerBytes, int spanElementOptions = 0);

    /* Check for existence of a span element */
    bool containsSpanElement(int typeId) const;

    /* Remove and free a span element handler in the registry */
    void removeSpanElement(int typeId);

    /* Querying span elements */
    int numberOfSpanElementsForTriggerByte(char byte) const;
    VfmdSpanElementHandler *spanElementForTriggerByteAtIndex(char byte, unsigned int index) const;
    int spanOptionsForTriggerByteAtIndex(char byte, unsigned int index) const;
    int indexOfSpanTriggerByteIn(const VfmdByteArray &ba, int offset) const;

    void print() const;

    // Default vfmd elements

    static VfmdElementRegistry* createRegistryForCoreSyntax();

private:
    /* Prevent copying of this class */
    VfmdElementRegistry(const VfmdElementRegistry& other);
    VfmdElementRegistry& operator=(const VfmdElementRegistry& other);

    // Block elements

    void ensureBlockElementsAllocated();

    struct BlockElementData {
        BlockElementData(int t, VfmdBlockElementHandler *h)
            : typeId(t), blockElementHandler(h), canAbutParagraph(false) { }
        int typeId;
        VfmdBlockElementHandler *blockElementHandler;
        bool canAbutParagraph;
    };

    VfmdPointerArray<BlockElementData>* m_blockElements;

    // Span elements

    RegistryData<VfmdSpanElementHandler> *m_spanElementsData;

    friend void printBlockElementData(VfmdElementRegistry::BlockElementData *e);
};

#endif // VFMDSPANELEMENTREGISTRY_H
