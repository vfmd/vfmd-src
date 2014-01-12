#ifndef VFMDSPANELEMENTREGISTRY_H
#define VFMDSPANELEMENTREGISTRY_H

#include "vfmdpointerarray.h"
#include "vfmdbytearray.h"

class VfmdSpanElementHandler;
class VfmdBlockElementHandler;

class VfmdElementRegistry
{
public:
    enum SpanElementType {
        EMPHASIS_SPAN_ELEMENT,
        LINK_SPAN_ELEMENT,
        USER_SPAN_ELEMENT = 100,
        MAX_USER_SPAN_ELEMENT = 255
    };

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

    /* Querying block elements */
    unsigned int blockElementsCount() const;
    VfmdBlockElementHandler *blockElementHandler(unsigned int index) const;

    // Span elements

    /* Add a span element handler to the registry at the end.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler);

    /* Add a span element handler that should be invoked only when one of the triggerBytes are encountered.
     * Multiple span element handlers can be registered for a particular triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler, const VfmdByteArray &triggerBytes);

    /* Add a span element handler to the registry, overriding any other registered handler that may be present
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler);

    /* Add a span element handler that should be invoked only when one of the triggerBytes are encountered, so
     * that this handler is called before any other handler that may be present for the triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler, const VfmdByteArray &triggerBytes);

    /* Check for existence of a span element */
    bool containsSpanElement(int typeId) const;

    /* Remove and free a span element handler in the registry */
    void removeSpanElement(int typeId);

    /* Querying span elements that are associated with a trigger byte */
    int spanElementsWithoutTriggerByteCount() const;
    VfmdSpanElementHandler *spanElementWithoutTriggerByte(unsigned int index) const;

    /* Querying span elements that are not associated with a trigger byte */
    int spanElementCountForTriggerByte(char byte) const;
    VfmdSpanElementHandler *spanElementForTriggerByte(char byte, unsigned int index) const;

    void print() const;

private:

    // Block elements

    void ensureBlockElementsAllocated();

    struct BlockElementData {
        BlockElementData(int t, VfmdBlockElementHandler *h) : typeId(t), blockElementHandler(h) { }
        int typeId;
        VfmdBlockElementHandler *blockElementHandler;
    };

    VfmdPointerArray<BlockElementData>* m_blockElements;

    // Span elements

    struct SpanElementData {
        SpanElementData(int t, VfmdSpanElementHandler *h) : typeId(t), spanElementHandler(h) { }
        int typeId;
        VfmdSpanElementHandler *spanElementHandler;
    };

    void ensureSpanElementsWithoutTriggerByteAllocated();
    void ensureSpanElementsForTriggerByteAllocated(char c);
    bool doesSpanElementExistInPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const;
    void removeSpanElementFromPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const;

    VfmdPointerArray<SpanElementData>* m_spanElementsWithoutTriggerByte;
    VfmdPointerArray<SpanElementData>* m_spanElementsByTriggerByte[256];
    VfmdByteArray* m_triggerBytesById[256];
};

#endif // VFMDSPANELEMENTREGISTRY_H
