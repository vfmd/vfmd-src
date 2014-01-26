#include "vfmdelementregistry.h"
#include "spanelements/vfmdspanelementhandler.h"
#include "blockelements/vfmdblockelementhandler.h"
#include <stdio.h>

VfmdElementRegistry::VfmdElementRegistry()
{
    m_blockElements = 0;
    for (int i = 0; i < 256; i++) {
        m_spanElementsByTriggerByte[i] = 0;
        m_triggerBytesById[i] = 0;
    }
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
    for (int typeId = 0; typeId < 256; typeId++) {
        VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
        if (triggerBytes == 0 || triggerBytes->size() == 0) {
            // This is a not valid typeId. Nothing to free.
            continue;
        }
        VfmdPointerArray<SpanElementData>* spanElementsArray = m_spanElementsByTriggerByte[(unsigned char) triggerBytes->charAt(0)];
        int triggerBytesCount = triggerBytes->size();
        for (int i = 0; i < triggerBytesCount; i++) {
            VfmdElementRegistry::SpanElementData *spanElementData = spanElementsArray->itemAt(i);
            if (typeId == spanElementData->typeId) {
                delete spanElementData->spanElementHandler;
                delete spanElementData;
                break;
            }
        }
        delete triggerBytes;
    }

    // Free span element handler arrays
    for (int triggerByte = 0; triggerByte < 256; triggerByte++) {
        delete m_spanElementsByTriggerByte[(unsigned char) triggerByte];
    }
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

unsigned int VfmdElementRegistry::blockElementsCount() const
{
    return m_blockElements->size();
}

VfmdBlockElementHandler *VfmdElementRegistry::blockElementHandler(unsigned int index) const
{
    return m_blockElements->itemAt(index)->blockElementHandler;
}

// Span elements

void VfmdElementRegistry::ensureSpanElementsForTriggerByteAllocated(char byte)
{
    if (m_spanElementsByTriggerByte[(unsigned char) byte] == 0) {
        m_spanElementsByTriggerByte[(unsigned char) byte] = new VfmdPointerArray<SpanElementData>(8);
    }
}

bool VfmdElementRegistry::appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                            const VfmdByteArray &triggerBytes, int triggerOptions)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }

    if (triggerBytes.size() == 0) {
        return false;
    }

    m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
    VfmdElementRegistry::SpanElementData *spanElementData = new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler, triggerOptions);
    for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
        char byte = triggerBytes.charAt(byteIndex);
        ensureSpanElementsForTriggerByteAllocated(byte);
        m_spanElementsByTriggerByte[(unsigned char) byte]->append(spanElementData);
    }
    return true;
}

bool VfmdElementRegistry::prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler,
                                             const VfmdByteArray &triggerBytes, int triggerOptions)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }

    if (triggerBytes.size() == 0) {
        return false;
    }

    m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
    VfmdElementRegistry::SpanElementData *spanElementData = new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler, triggerOptions);
    for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
        char byte = triggerBytes.charAt(byteIndex);
        ensureSpanElementsForTriggerByteAllocated(byte);
        m_spanElementsByTriggerByte[(unsigned char) byte]->prepend(spanElementData);
    }
    return true;
}

bool VfmdElementRegistry::doesSpanElementExistInPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const
{
    int count = (array? array->size() : 0);
    for (int i = 0; i < count; i++) {
        VfmdElementRegistry::SpanElementData *spanElementData = array->itemAt(i);
        if (typeId == spanElementData->typeId) {
            return true;
        }
    }
    return false;
}

void VfmdElementRegistry::removeSpanElementFromPointerArray(int typeId, VfmdPointerArray<VfmdElementRegistry::SpanElementData> *array) const
{
    if (array) {
        for (unsigned int i = 0; i < array->size(); i++) {
            VfmdElementRegistry::SpanElementData *spanElementData = array->itemAt(i);
            if (typeId == spanElementData->typeId) {
                VfmdElementRegistry::SpanElementData *spanElementData = array->takeItemAt(i);
                delete spanElementData->spanElementHandler;
                delete spanElementData;
                return;
            }
        }
    }
}

bool VfmdElementRegistry::containsSpanElement(int typeId) const
{
    VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
    if (triggerBytes && triggerBytes->size() > 0) {
        return doesSpanElementExistInPointerArray(typeId, m_spanElementsByTriggerByte[(unsigned char) triggerBytes->charAt(0)]);
    }
    return false;
}

void VfmdElementRegistry::removeSpanElement(int typeId)
{
    VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
    if (triggerBytes && triggerBytes->size() > 0) {
        for (unsigned int i = 0; i < triggerBytes->size(); i++) {
            removeSpanElementFromPointerArray(typeId, m_spanElementsByTriggerByte[(unsigned char) triggerBytes->charAt(i)]);
        }
    }
}

int VfmdElementRegistry::spanElementCountForTriggerByte(char byte) const
{
    VfmdPointerArray<VfmdElementRegistry::SpanElementData>* spanElements = m_spanElementsByTriggerByte[(unsigned char) byte];
    return (spanElements? spanElements->size() : 0);
}

VfmdSpanElementHandler *VfmdElementRegistry::spanElementForTriggerByte(char byte, unsigned int index) const
{
    VfmdPointerArray<VfmdElementRegistry::SpanElementData>* spanElements = m_spanElementsByTriggerByte[(unsigned char) byte];
    return (spanElements? spanElements->itemAt(index)->spanElementHandler : 0);
}

int VfmdElementRegistry::triggerOptionsForTriggerByte(char byte, unsigned int index) const
{
    VfmdPointerArray<VfmdElementRegistry::SpanElementData>* spanElements = m_spanElementsByTriggerByte[(unsigned char) byte];
    return (spanElements? spanElements->itemAt(index)->triggerOptions : 0);
}

void printSpanElementData(VfmdElementRegistry::SpanElementData *e)
{
    if (e->triggerOptions) {
        printf("%s (id: %d, options: 0x%x)  ", e->spanElementHandler->description(), e->typeId, e->triggerOptions);
    } else {
        printf("%s (id: %d)  ", e->spanElementHandler->description(), e->typeId);
    }
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
    for (unsigned int byte = 0; byte < 256; byte++) {
        VfmdPointerArray<VfmdElementRegistry::SpanElementData>* spanElements = m_spanElementsByTriggerByte[(unsigned char) byte];
        if (spanElements) {
            printf("  Triggerbyte '%c': ", byte);
            spanElements->map(printSpanElementData);
            printf("\n");
        }
    }
    printf("\n");
}
