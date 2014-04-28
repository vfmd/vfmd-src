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

    enum BlockElementOptions {
        /* By default, for a block to start immediately after a paragraph,
         * it needs to be separated from the paragraph by a blank line.
         * Use this option to tell the registry that the block may start
         * after a paragraph even without the separating blank line.
         * This option can be used only if the handler has atleast one triggerByte. */
        BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH = 1
    };

    /* Add a block element handler that should be invoked only when one of the triggerBytes
     * forms the first non-space byte of a line. If no triggerBytes are provided,
     * the handler is invoked for all block-starting lines irrespective of the first
     * non-space byte. For a given line, the non-triggerBytes-specific handlers are
     * invoked before the triggerBytes-specific handlers, if any.
     * Multiple block element handlers can be registered for a particular triggerByte.
     * The registry owns the added handler.
     * If typeId is already registered, this method does nothing and returns false. */
    bool appendBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler,
                           const VfmdByteArray &triggerBytes = VfmdByteArray(), int blockElementOptions = 0);

    /* Same as 'appendBlockElement()', except that this handler is registered such that
     * it's invoked before any previously registered handlers. */
    bool prependBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler,
                            const VfmdByteArray &triggerBytes = VfmdByteArray(), int blockElementOptions = 0);

    /* Check for existence of a block element */
    bool containsBlockElement(int typeId) const;

    /* Remove and free a block element handler in the registry */
    void removeBlockElement(int typeId);

    /* Replace a block element handler.
     * The existing handler that's getting replaced will be freed. */
    void replaceBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler);

    /* Querying block elements */
    int numberOfBlockElementsForTriggerByte(char byte) const;
    VfmdBlockElementHandler *blockElementForTriggerByteAtIndex(char byte, unsigned int index) const;
    int blockOptionsForTriggerByteAtIndex(char byte, unsigned int index) const;

    int numberOfBlockElementsWithoutAnyTriggerByte() const;
    VfmdBlockElementHandler *blockElementWithoutAnyTriggerByteAtIndex(unsigned int index) const;
    int blockOptionsWithoutAnyTriggerByteAtIndex(unsigned int index) const;

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

    /* Replace a span element handler.
     * The existing handler that's getting replaced will be freed. */
    void replaceSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler);

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

    /* Member variables */
    RegistryData<VfmdBlockElementHandler> *m_blockElementsData;
    RegistryData<VfmdSpanElementHandler> *m_spanElementsData;
};

#endif // VFMDSPANELEMENTREGISTRY_H
