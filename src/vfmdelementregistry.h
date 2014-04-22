#ifndef VFMDSPANELEMENTREGISTRY_H
#define VFMDSPANELEMENTREGISTRY_H

#include "core/vfmdpointerarray.h"
#include "vfmdbytearray.h"

class VfmdSpanElementHandler;
class VfmdBlockElementHandler;
class VfmdLinkRefMap;

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

    enum TriggerOptionsForPositionOfInvocation {

        /* TRIGGER_AT_TRIGGER_BYTE:
         * Invoke the handler when the current byte is a trigger byte.
         * This is the default option. */
        TRIGGER_AT_TRIGGER_BYTE = 0,

        /* TRIGGER_BEFORE_TRIGGER_BYTE:
         * Invoke the handler when a trigger byte is at or ahead of
         * the current byte and there are no handlers to invoke
         * between the current byte and the next trigger byte. */
        TRIGGER_BEFORE_TRIGGER_BYTE = 1
    };

    enum TriggerOptionsForEscaping {

        /* TRIGGER_ONLY_IF_UNESCAPED:
         * Invoke the handler only if the trigger byte is unescaped.
         * This is the default option. */
        TRIGGER_ONLY_IF_UNESCAPED = 0,

        /* TRIGGER_EVEN_IF_ESCAPED:
         * Invoke the handler even if the trigger byte is escaped. */
        TRIGGER_EVEN_IF_ESCAPED = 8
    };

    /* Add a span element handler that should be invoked only when one of the triggerBytes is encountered.
     * Multiple span element handlers can be registered for a particular triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                           const VfmdByteArray &triggerBytes, int triggerOptions = 0);

    /* Add a span element handler that should be invoked only when one of the triggerBytes is encountered, so
     * that this handler is called before any other handler that may be present for the triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                            const VfmdByteArray &triggerBytes, int triggerOptions = 0);

    /* Check for existence of a span element */
    bool containsSpanElement(int typeId) const;

    /* Remove and free a span element handler in the registry */
    void removeSpanElement(int typeId);

    /* Querying span elements */
    int spanElementCountForTriggerByte(char byte) const;
    VfmdSpanElementHandler *spanElementForTriggerByte(char byte, unsigned int index) const;
    int triggerOptionsForTriggerByte(char byte, unsigned int index) const;
    int indexOfTriggerByteIn(const VfmdByteArray &ba, int offset) const;

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

    struct SpanElementData {
        SpanElementData(int t, VfmdSpanElementHandler *h, int to)
            : typeId(t), spanElementHandler(h), triggerOptions(to) { }
        int typeId;
        VfmdSpanElementHandler *spanElementHandler;
        int triggerOptions;
    };

    void ensureSpanElementsForTriggerByteAllocated(char c);
    bool doesSpanElementExistInPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const;
    void removeSpanElementFromPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const;

    VfmdPointerArray<SpanElementData>* m_spanElementsByTriggerByte[256];
    VfmdByteArray* m_triggerBytesById[256];

    friend void printSpanElementData(VfmdElementRegistry::SpanElementData *e);
    friend void printBlockElementData(VfmdElementRegistry::BlockElementData *e);
};

#endif // VFMDSPANELEMENTREGISTRY_H
