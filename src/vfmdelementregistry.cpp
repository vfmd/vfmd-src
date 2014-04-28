#include "vfmdelementregistry.h"
#include "vfmdspanelementhandler.h"
#include "vfmdblockelementhandler.h"
#include <stdio.h>

template<class T>
class RegistryData
{
public:
    struct ElementData {
        ElementData(int t, T *h, int to)
            : typeId(t), elementHandler(h), options(to) { }
        int typeId;
        T *elementHandler;
        int options;

        void print() {
            if (options) {
                printf("[%s: %d (options: 0x%x)] ", elementHandler->description(), typeId, options);
            } else {
                printf("[%s: %d] ", elementHandler->description(), typeId);
            }
        }
    };

    RegistryData() {
        m_elementsWithoutAnyTriggerByte = 0;
        for (int i = 0; i < 256; i++) {
            m_elementsByTriggerByte[i] = 0;
            m_triggerBytesById[i] = 0;
        }
    }

    ~RegistryData() {
        for (int typeId = 0; typeId < 256; typeId++) {
            VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
            if (triggerBytes == 0 || triggerBytes->size() == 0) {
                // This is a not valid typeId. Nothing to free.
                continue;
            }
            VfmdPointerArray<ElementData>* elementsArray = m_elementsByTriggerByte[(unsigned char) triggerBytes->byteAt(0)];
            int triggerBytesCount = triggerBytes->size();
            for (int i = 0; i < triggerBytesCount; i++) {
                ElementData *elementData = elementsArray->itemAt(i);
                if (typeId == elementData->typeId) {
                    delete elementData->elementHandler;
                    delete elementData;
                    break;
                }
            }
            delete triggerBytes;
        }
        for (int triggerByte = 0; triggerByte < 256; triggerByte++) {
            delete m_elementsByTriggerByte[(unsigned char) triggerByte];
        }
        if (m_elementsWithoutAnyTriggerByte) {
            int triggerBytesCount = m_elementsWithoutAnyTriggerByte->size();
            for (int i = 0; i < triggerBytesCount; i++) {
                ElementData *elementData = m_elementsWithoutAnyTriggerByte->itemAt(i);
                delete elementData->elementHandler;
                delete elementData;
                break;
            }
            delete m_elementsWithoutAnyTriggerByte;
        }
    }

    bool append(int typeId, T *elementHandler, const VfmdByteArray &triggerBytes, int options = 0) {
        if (m_triggerBytesById[typeId] != 0) {
            // Cannot add the same element twice
            return false;
        }
        ElementData *elementData = new ElementData(typeId, elementHandler, options);
        if (triggerBytes.size() > 0) {
            m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
            for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
                assert(triggerBytes.isUTF8CharStartingAt(byteIndex));
                char byte = triggerBytes.byteAt(byteIndex);
                ensureElementsForTriggerByteAllocated(byte);
                m_elementsByTriggerByte[(unsigned char) byte]->append(elementData);
            }
        } else {
            ensureElementsWithoutAnyTriggerByteAllocated();
            m_elementsWithoutAnyTriggerByte->append(elementData);
        }
        return true;
    }

    bool prepend(int typeId, T *elementHandler, const VfmdByteArray &triggerBytes, int options = 0) {
        if (m_triggerBytesById[typeId] != 0) {
            // Cannot add the same element twice
            return false;
        }
        ElementData *elementData = new ElementData(typeId, elementHandler, options);
        if (triggerBytes.size() > 0) {
            m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
            for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
                assert(triggerBytes.isUTF8CharStartingAt(byteIndex));
                char byte = triggerBytes.byteAt(byteIndex);
                ensureElementsForTriggerByteAllocated(byte);
                m_elementsByTriggerByte[(unsigned char) byte]->prepend(elementData);
            }
        } else {
            ensureElementsWithoutAnyTriggerByteAllocated();
            m_elementsWithoutAnyTriggerByte->prepend(elementData);
        }
        return true;
    }

    bool contains(int typeId) const {
        VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
        if (triggerBytes && triggerBytes->size() > 0) {
            return RegistryData::doesElementExistInPointerArray(typeId, m_elementsByTriggerByte[(unsigned char) triggerBytes->byteAt(0)]);
        }
        return RegistryData::doesElementExistInPointerArray(typeId, m_elementsWithoutAnyTriggerByte);
    }

    void remove(int typeId) {
        VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
        if (triggerBytes && triggerBytes->size() > 0) {
            for (unsigned int i = 0; i < triggerBytes->size(); i++) {
                RegistryData::removeElementFromPointerArray(typeId, m_elementsByTriggerByte[(unsigned char) triggerBytes->byteAt(i)]);
            }
        }
        if (m_elementsWithoutAnyTriggerByte) {
            RegistryData::removeElementFromPointerArray(typeId, m_elementsWithoutAnyTriggerByte);
        }
    }

    void replace(int typeId, T *handler) {
        VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
        T* existingHandler = 0;
        if (triggerBytes && triggerBytes->size() > 0) {
            existingHandler = RegistryData::replaceElementInPointerArray(
                                   typeId,
                                   m_elementsByTriggerByte[(unsigned char) triggerBytes->byteAt(0)],
                                   handler);
            for (unsigned int i = 1; i < triggerBytes->size(); i++) {
                T *h = RegistryData::replaceElementInPointerArray(
                            typeId,
                            m_elementsByTriggerByte[(unsigned char) triggerBytes->byteAt(i)],
                            handler);
                assert(existingHandler == h);
            }
        } else {
            existingHandler = RegistryData::replaceElementInPointerArray(
                                   typeId,
                                   m_elementsWithoutAnyTriggerByte,
                                   handler);
        }
        delete existingHandler;
    }

    int numberOfElementsForTriggerByte(char byte) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->size() : 0);
    }

    T *elementForTriggerByte(char byte, unsigned int index) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->itemAt(index)->elementHandler : 0);
    }

    int optionsForTriggerByte(char byte, unsigned int index) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->itemAt(index)->options : 0);
    }

    int indexOfTriggerByteIn(const VfmdByteArray &ba, int offset) const {
        int i = offset;
        const unsigned int sz = ba.size();
        const char *p = ba.data() + i;
        while (i < sz) {
            if (m_elementsByTriggerByte[(unsigned char) (*p)] != 0) {
                return i;
            }
            p++;
            i++;
        }
        return sz;
    }

    int numberOfElementsWithoutAnyTriggerByte() const {
        return (m_elementsWithoutAnyTriggerByte? m_elementsWithoutAnyTriggerByte->size() : 0);
    }

    T *elementWithoutAnyTriggerByte(unsigned int index) const {
        return (m_elementsWithoutAnyTriggerByte? m_elementsWithoutAnyTriggerByte->itemAt(index)->elementHandler : 0);
    }

    int optionsWithoutAnyTriggerByte(unsigned int index) const {
        return (m_elementsWithoutAnyTriggerByte? m_elementsWithoutAnyTriggerByte->itemAt(index)->options : 0);
    }

    void ensureElementsForTriggerByteAllocated(char c) {
        if (m_elementsByTriggerByte[(unsigned char) c] == 0) {
            m_elementsByTriggerByte[(unsigned char) c] = new VfmdPointerArray<ElementData>(8);
        }
    }

    void ensureElementsWithoutAnyTriggerByteAllocated() {
        if (m_elementsWithoutAnyTriggerByte == 0) {
            m_elementsWithoutAnyTriggerByte = new VfmdPointerArray<ElementData>(8);
        }
    }

    void print() const {
        if (m_elementsWithoutAnyTriggerByte && m_elementsWithoutAnyTriggerByte->size() > 0) {
            printf("  No trigger byte: ");
            for (int i = 0; i < m_elementsWithoutAnyTriggerByte->size(); i++) {
                m_elementsWithoutAnyTriggerByte->itemAt(i)->print();
            }
            printf("\n");
        }
        for (unsigned int byte = 0; byte < 256; byte++) {
            VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
            if (elements) {
                printf("  Triggerbyte '%c': ", byte);
                for (int i = 0; i < elements->size(); i++) {
                    elements->itemAt(i)->print();
                }
                printf("\n");
            }
        }
        printf("\n");
    }

    static bool doesElementExistInPointerArray(int typeId, VfmdPointerArray<ElementData> *array) {
        int count = (array? array->size() : 0);
        for (int i = 0; i < count; i++) {
            ElementData *elementData = array->itemAt(i);
            if (typeId == elementData->typeId) {
                return true;
            }
        }
        return false;
    }

    static void removeElementFromPointerArray(int typeId, VfmdPointerArray<ElementData> *array) {
        if (array) {
            for (unsigned int i = 0; i < array->size(); i++) {
                ElementData *elementData = array->itemAt(i);
                if (typeId == elementData->typeId) {
                    ElementData *elementData = array->takeItemAt(i);
                    delete elementData->elementHandler;
                    delete elementData;
                    return;
                }
            }
        }
    }

    static T* replaceElementInPointerArray(int typeId, VfmdPointerArray<ElementData> *array, T *replacingHandler) {
        if (array) {
            for (unsigned int i = 0; i < array->size(); i++) {
                ElementData *elementData = array->itemAt(i);
                if (typeId == elementData->typeId) {
                    T *replacedHandler = elementData->elementHandler;
                    elementData->elementHandler = replacingHandler;
                    return replacedHandler;
                }
            }
        }
        return 0;
    }

    VfmdPointerArray<ElementData>* m_elementsByTriggerByte[256];
    VfmdByteArray* m_triggerBytesById[256];
    VfmdPointerArray<ElementData>* m_elementsWithoutAnyTriggerByte;
};

VfmdElementRegistry::VfmdElementRegistry()
{
    m_blockElementsData = new RegistryData<VfmdBlockElementHandler>;
    m_spanElementsData = new RegistryData<VfmdSpanElementHandler>;
}

VfmdElementRegistry::~VfmdElementRegistry()
{
    delete m_blockElementsData;
    delete m_spanElementsData;
}

// Block elements

bool VfmdElementRegistry::appendBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler,
                                             const VfmdByteArray &triggerBytes, int blockElementOptions)
{
    if ((triggerBytes.size() == 0) &&
        ((blockElementOptions & BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH) == BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH)) {
        // The BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH option requires trigger bytes
        assert(false);
        return false;
    }
    return m_blockElementsData->append(typeId, blockElementHandler, triggerBytes, blockElementOptions);
}

bool VfmdElementRegistry::prependBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler,
                                              const VfmdByteArray &triggerBytes, int blockElementOptions)
{
    if ((triggerBytes.size() == 0) &&
        ((blockElementOptions & BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH) == BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH)) {
        // The BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH option requires trigger bytes
        assert(false);
        return false;
    }
    return m_blockElementsData->prepend(typeId, blockElementHandler, triggerBytes, blockElementOptions);
}

bool VfmdElementRegistry::containsBlockElement(int typeId) const
{
    return m_blockElementsData->contains(typeId);
}

void VfmdElementRegistry::removeBlockElement(int typeId)
{
    return m_blockElementsData->remove(typeId);
}

void VfmdElementRegistry::replaceBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler)
{
    m_blockElementsData->replace(typeId, blockElementHandler);
}

int VfmdElementRegistry::numberOfBlockElementsForTriggerByte(char byte) const
{
    return m_blockElementsData->numberOfElementsForTriggerByte(byte);
}

VfmdBlockElementHandler *VfmdElementRegistry::blockElementForTriggerByteAtIndex(char byte, unsigned int index) const
{
    return m_blockElementsData->elementForTriggerByte(byte, index);
}

int VfmdElementRegistry::blockOptionsForTriggerByteAtIndex(char byte, unsigned int index) const
{
    return m_blockElementsData->optionsForTriggerByte(byte, index);
}

int VfmdElementRegistry::numberOfBlockElementsWithoutAnyTriggerByte() const
{
    return m_blockElementsData->numberOfElementsWithoutAnyTriggerByte();
}

VfmdBlockElementHandler *VfmdElementRegistry::blockElementWithoutAnyTriggerByteAtIndex(unsigned int index) const
{
    return m_blockElementsData->elementWithoutAnyTriggerByte(index);
}

int VfmdElementRegistry::blockOptionsWithoutAnyTriggerByteAtIndex(unsigned int index) const
{
    return m_blockElementsData->optionsWithoutAnyTriggerByte(index);
}

// Span elements

bool VfmdElementRegistry::appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                            const VfmdByteArray &triggerBytes, int spanElementOptions)
{
    if (triggerBytes.size() == 0) {
        // Don't allow span elements without trigger-bytes
        return false;
    }
    return m_spanElementsData->append(typeId, spanElementHandler, triggerBytes, spanElementOptions);
}

bool VfmdElementRegistry::prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                             const VfmdByteArray &triggerBytes, int spanElementOptions)
{
    if (triggerBytes.size() == 0) {
        // Don't allow span elements without trigger-bytes
        return false;
    }
    return m_spanElementsData->prepend(typeId, spanElementHandler, triggerBytes, spanElementOptions);
}

bool VfmdElementRegistry::containsSpanElement(int typeId) const
{
    return m_spanElementsData->contains(typeId);
}

void VfmdElementRegistry::removeSpanElement(int typeId)
{
    m_spanElementsData->remove(typeId);
}

void VfmdElementRegistry::replaceSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler)
{
    m_spanElementsData->replace(typeId, spanElementHandler);
}

int VfmdElementRegistry::numberOfSpanElementsForTriggerByte(char byte) const
{
    return m_spanElementsData->numberOfElementsForTriggerByte(byte);
}

VfmdSpanElementHandler *VfmdElementRegistry::spanElementForTriggerByteAtIndex(char byte, unsigned int index) const
{
    return m_spanElementsData->elementForTriggerByte(byte, index);
}

int VfmdElementRegistry::spanOptionsForTriggerByteAtIndex(char byte, unsigned int index) const
{
    return m_spanElementsData->optionsForTriggerByte(byte, index);
}

int VfmdElementRegistry::indexOfSpanTriggerByteIn(const VfmdByteArray &ba, int offset) const
{
    return m_spanElementsData->indexOfTriggerByteIn(ba, offset);
}

void VfmdElementRegistry::print() const
{
    printf("Block element handlers:\n");
    m_blockElementsData->print();
    printf("Span element handlers:\n");
    m_spanElementsData->print();
    printf("\n");
}
