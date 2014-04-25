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
            : typeId(t), elementHandler(h), triggerOptions(to) { }
        int typeId;
        T *elementHandler;
        int triggerOptions;

        void print() {
            if (triggerOptions) {
                printf("%s (id: %d, options: 0x%x)  ", elementHandler->description(), typeId, triggerOptions);
            } else {
                printf("%s (id: %d)  ", elementHandler->description(), typeId);
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

    bool append(int typeId, T *elementHandler, const VfmdByteArray &triggerBytes, int triggerOptions = 0) {
        if (m_triggerBytesById[typeId] != 0) {
            // Cannot add the same element twice
            return false;
        }
        ElementData *elementData = new ElementData(typeId, elementHandler, triggerOptions);
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

    bool prepend(int typeId, T *elementHandler, const VfmdByteArray &triggerBytes, int triggerOptions = 0) {
        if (m_triggerBytesById[typeId] != 0) {
            // Cannot add the same element twice
            return false;
        }
        ElementData *elementData = new ElementData(typeId, elementHandler, triggerOptions);
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
    }

    int numberOfElementsForTriggerByte(char byte) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->size() : 0);
    }

    T *elementForTriggerByte(char byte, unsigned int index) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->itemAt(index)->elementHandler : 0);
    }

    int triggerOptionsForTriggerByte(char byte, unsigned int index) const {
        VfmdPointerArray<ElementData>* elements = m_elementsByTriggerByte[(unsigned char) byte];
        return (elements? elements->itemAt(index)->triggerOptions : 0);
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
        printf("  No trigger byte: ");
        for (int i = 0; i < m_elementsWithoutAnyTriggerByte->size(); i++) {
            m_elementsWithoutAnyTriggerByte->itemAt(i)->print();
        }
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

    VfmdPointerArray<ElementData>* m_elementsByTriggerByte[256];
    VfmdByteArray* m_triggerBytesById[256];
    VfmdPointerArray<ElementData>* m_elementsWithoutAnyTriggerByte;
};

VfmdElementRegistry::VfmdElementRegistry()
{
    m_blockElements = 0;
    m_spanElementsData = new RegistryData<VfmdSpanElementHandler>;
}

VfmdElementRegistry::~VfmdElementRegistry()
{
    // Free block element handlers
    if (m_blockElements) {
        for (unsigned int i = 0; i < m_blockElements->size(); i++) {
            VfmdElementRegistry::BlockElementData *blockElementData = m_blockElements->itemAt(i);
            delete blockElementData->blockElementHandler;
            delete blockElementData;
        }
        delete m_blockElements;
    }

    // Free span element handlers
    delete m_spanElementsData;
}

// Block elements

void VfmdElementRegistry::ensureBlockElementsAllocated()
{
    if (m_blockElements == 0) {
        m_blockElements = new VfmdPointerArray<BlockElementData>(16);
    }
}

bool VfmdElementRegistry::appendBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler)
{
    if (containsBlockElement(typeId)) {
        return false;
    }
    ensureBlockElementsAllocated();
    m_blockElements->append(new VfmdElementRegistry::BlockElementData(typeId, blockElementHandler));
    return true;
}

bool VfmdElementRegistry::insertBlockElementBeforeExistingBlockElement(int typeId, VfmdBlockElementHandler *blockElementHandler, int existingTypeId)
{
    if (containsBlockElement(typeId)) {
        return false;
    }
    ensureBlockElementsAllocated();
    for (unsigned int i = 0; i < m_blockElements->size(); i++) {
        VfmdElementRegistry::BlockElementData *blockElementData = m_blockElements->itemAt(i);
        if (blockElementData->typeId == existingTypeId) {
            m_blockElements->insert(new VfmdElementRegistry::BlockElementData(typeId, blockElementHandler), i);
            return true;
        }
    }
    return false;
}

int VfmdElementRegistry::indexOfBlockElement(int typeId) const
{
    for (unsigned int i = 0; i < m_blockElements->size(); i++) {
        VfmdElementRegistry::BlockElementData *blockElementData = m_blockElements->itemAt(i);
        if (blockElementData->typeId == typeId) {
            return (int) i;
        }
    }
    return -1;
}

bool VfmdElementRegistry::containsBlockElement(int typeId) const
{
    return ((m_blockElements != 0) && (indexOfBlockElement(typeId) >= 0));
}

void VfmdElementRegistry::removeBlockElement(int typeId)
{
    int index = indexOfBlockElement(typeId);
    m_blockElements->removeItemAt((unsigned int) index);
}

void VfmdElementRegistry::setBlockCanAbutParagraph(int typeId, bool yes)
{
    int index = indexOfBlockElement(typeId);
    if (index >= 0) {
        m_blockElements->itemAt(index)->canAbutParagraph = yes;
    }
}

VfmdPointerArray<VfmdBlockElementHandler>* VfmdElementRegistry::blockHandlersThatCanAbutParagraph() const
{
    VfmdPointerArray<VfmdBlockElementHandler> *blockHandlers = new VfmdPointerArray<VfmdBlockElementHandler>(16);
    for (unsigned int i = 0; i < m_blockElements->size(); i++) {
        const VfmdElementRegistry::BlockElementData *blockElementData = m_blockElements->itemAt(i);
        if (blockElementData->canAbutParagraph) {
            blockHandlers->append(blockElementData->blockElementHandler);
        }
    }
    return blockHandlers;
}

unsigned int VfmdElementRegistry::blockElementsCount() const
{
    return m_blockElements->size();
}

VfmdBlockElementHandler *VfmdElementRegistry::blockElementHandler(unsigned int index) const
{
    return m_blockElements->itemAt(index)->blockElementHandler;
}

// Span elements

bool VfmdElementRegistry::appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                            const VfmdByteArray &triggerBytes, int triggerOptions)
{
    if (triggerBytes.size() == 0) {
        // Don't allow span elements without trigger-bytes
        return false;
    }
    return m_spanElementsData->append(typeId, spanElementHandler, triggerBytes, triggerOptions);
}

bool VfmdElementRegistry::prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                             const VfmdByteArray &triggerBytes, int triggerOptions)
{
    if (triggerBytes.size() == 0) {
        // Don't allow span elements without trigger-bytes
        return false;
    }
    return m_spanElementsData->prepend(typeId, spanElementHandler, triggerBytes, triggerOptions);
}

bool VfmdElementRegistry::containsSpanElement(int typeId) const
{
    return m_spanElementsData->contains(typeId);
}

void VfmdElementRegistry::removeSpanElement(int typeId)
{
    m_spanElementsData->remove(typeId);
}

int VfmdElementRegistry::spanElementCountForTriggerByte(char byte) const
{
    return m_spanElementsData->numberOfElementsForTriggerByte(byte);
}

VfmdSpanElementHandler *VfmdElementRegistry::spanElementForTriggerByte(char byte, unsigned int index) const
{
    return m_spanElementsData->elementForTriggerByte(byte, index);
}

int VfmdElementRegistry::triggerOptionsForTriggerByte(char byte, unsigned int index) const
{
    return m_spanElementsData->triggerOptionsForTriggerByte(byte, index);
}

int VfmdElementRegistry::indexOfTriggerByteIn(const VfmdByteArray &ba, int offset) const
{
    return m_spanElementsData->indexOfTriggerByteIn(ba, offset);
}

void printBlockElementData(VfmdElementRegistry::BlockElementData *e)
{
    printf("  %s (id: %d)\n", e->blockElementHandler->description(), e->typeId);
}

void VfmdElementRegistry::print() const
{
    printf("Block element handlers:\n");
    m_blockElements->map(printBlockElementData);
    printf("Span element handlers:\n");
    m_spanElementsData->print();
    printf("\n");
}
