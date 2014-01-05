#include "vfmdelementregistry.h"
#include "spanelements/vfmdspanelementhandler.h"
#include <stdio.h>

VfmdElementRegistry::VfmdElementRegistry()
{
    m_spanElementsWithoutTriggerByte = 0;
    for (int i = 0; i < 256; i++) {
        m_spanElementsByTriggerByte[i] = 0;
        m_triggerBytesById[i] = 0;
    }
}

VfmdElementRegistry::~VfmdElementRegistry()
{
    for (int typeId = 0; typeId < 256; typeId++) {
        VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
        if (!triggerBytes) {
            // This is a not valid typeId. Nothing to free.
            continue;
        }
        int triggerBytesCount = triggerBytes->size();
        VfmdPointerArray<SpanElementData>* spanElementsArray = 0;
        if (triggerBytesCount> 0) {
            spanElementsArray = m_spanElementsByTriggerByte[(unsigned char) triggerBytes->charAt(0)];
        } else {
            spanElementsArray = m_spanElementsWithoutTriggerByte;
        }
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
}

void VfmdElementRegistry::ensureSpanElementsWithoutTriggerByteAllocated()
{
    if (m_spanElementsWithoutTriggerByte == 0) {
        m_spanElementsWithoutTriggerByte = new VfmdPointerArray<SpanElementData>(8);
    }
}

void VfmdElementRegistry::ensureSpanElementsForTriggerByteAllocated(char byte)
{
    if (m_spanElementsByTriggerByte[(unsigned char) byte] == 0) {
        m_spanElementsByTriggerByte[(unsigned char) byte] = new VfmdPointerArray<SpanElementData>(8);
    }
}

bool VfmdElementRegistry::appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }
    m_triggerBytesById[typeId] = new VfmdByteArray();
    ensureSpanElementsWithoutTriggerByteAllocated();
    m_spanElementsWithoutTriggerByte->append(new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler));
    return true;
}

bool VfmdElementRegistry::appendSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler, const VfmdByteArray &triggerBytes)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }
    if (triggerBytes.isValid() && triggerBytes.size() > 0) {
        m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
        VfmdElementRegistry::SpanElementData *spanElementData = new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler);
        for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
            char byte = triggerBytes.charAt(byteIndex);
            ensureSpanElementsForTriggerByteAllocated(byte);
            m_spanElementsByTriggerByte[(unsigned char) byte]->append(spanElementData);
        }
    } else {
        appendSpanElement(typeId, spanElementHandler);
    }
    return true;
}

bool VfmdElementRegistry::prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }
    m_triggerBytesById[typeId] = new VfmdByteArray();
    ensureSpanElementsWithoutTriggerByteAllocated();
    m_spanElementsWithoutTriggerByte->prepend(new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler));
    return true;
}

bool VfmdElementRegistry::prependSpanElement(int typeId, VfmdSpanElementHandler *spanElementHandler, const VfmdByteArray &triggerBytes)
{
    if (m_triggerBytesById[typeId] != 0) {
        return false;
    }
    if (triggerBytes.size() > 0) {
        m_triggerBytesById[typeId] = new VfmdByteArray(triggerBytes);
        VfmdElementRegistry::SpanElementData *spanElementData = new VfmdElementRegistry::SpanElementData(typeId, spanElementHandler);
        for (unsigned int byteIndex = 0; byteIndex < triggerBytes.size(); byteIndex++) {
            char byte = triggerBytes.charAt(byteIndex);
            ensureSpanElementsForTriggerByteAllocated(byte);
            m_spanElementsByTriggerByte[(unsigned char) byte]->prepend(spanElementData);
        }
    } else {
        prependSpanElement(typeId, spanElementHandler);
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
                array->removeItemAt(i);
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
    return doesSpanElementExistInPointerArray(typeId, m_spanElementsWithoutTriggerByte);
}

void VfmdElementRegistry::removeSpanElement(int typeId)
{
    VfmdByteArray *triggerBytes = m_triggerBytesById[typeId];
    if (triggerBytes && triggerBytes->size() > 0) {
        for (unsigned int i = 0; i < triggerBytes->size(); i++) {
            removeSpanElementFromPointerArray(typeId, m_spanElementsByTriggerByte[(unsigned char) triggerBytes->charAt(i)]);
        }
    } else {
        removeSpanElementFromPointerArray(typeId, m_spanElementsWithoutTriggerByte);
    }
}

int VfmdElementRegistry::spanElementsWithoutTriggerByteCount() const
{
    return (m_spanElementsWithoutTriggerByte? m_spanElementsWithoutTriggerByte->size() : 0);
}

VfmdSpanElementHandler *VfmdElementRegistry::spanElementWithoutTriggerByte(unsigned int index) const
{
    return (m_spanElementsWithoutTriggerByte? m_spanElementsWithoutTriggerByte->itemAt(index)->spanElementHandler : 0);
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

void VfmdElementRegistry::print() const
{
#if defined(__clang__) && __has_feature(cxx_lambdas)
    printf("VfmdElementRegistry:\n");
    for (unsigned int byte = 0; byte < 256; byte++) {
        VfmdPointerArray<VfmdElementRegistry::SpanElementData>* spanElements = m_spanElementsByTriggerByte[(unsigned char) byte];
        if (spanElements) {
            printf("  Triggerbyte '%c': ", byte);
            spanElements->map([](VfmdElementRegistry::SpanElementData *e) {
                              printf("%s(%d) ", e->spanElementHandler->description(), e->typeId);
                              });
            printf("\n");
        }
    }
    if (m_spanElementsWithoutTriggerByte && m_spanElementsWithoutTriggerByte->size() > 0) {
        printf("  No Triggerbyte : ");
        m_spanElementsWithoutTriggerByte->map([](VfmdElementRegistry::SpanElementData *e) {
                                              printf("%s(%d) ", e->spanElementHandler->description(), e->typeId);
                                              });
        printf("\n");
    }
    printf("\n");
#else
    printf("Printing the VfmdElementRegistry requires compiling with clang/C++11\n");
#endif
}
