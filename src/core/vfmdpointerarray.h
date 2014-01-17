#ifndef VFMDPOINTERARRAY_H
#define VFMDPOINTERARRAY_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

template<typename T>
class VfmdPointerArray
{
public:
    VfmdPointerArray(int reservedSize) {
        m_data = static_cast<T**>(malloc(sizeof(T*) * reservedSize));
        m_size = 0;
        m_allocatedSize = reservedSize;
        m_chunkSize = reservedSize;
    }

    ~VfmdPointerArray() {
        free(m_data);
    }

    void setAllocationChunkSize(unsigned int chunkSize) {
        m_chunkSize = chunkSize;
    }

    unsigned int size() const {
        return m_size;
    }

    T* itemAt(unsigned int index) const {
        return m_data[index];
    }

    void append(T *item) {
        if (m_size >= m_allocatedSize) {
            grow();
        }
        assert(m_size < m_allocatedSize);
        m_data[m_size] = item;
        m_size++;
    }

    void insert(T *item, unsigned int index) {
        if (m_size < m_allocatedSize) { // if there's sufficient space
            // right-shift the suceeding items
            for (unsigned int i = m_size - 1; i >= index; i--) {
                m_data[i+1] = m_data[i];
            }
            // assign the inserted item
            m_data[index] = item;
            m_size++;
        } else {
            // allocate new space
            m_allocatedSize += m_chunkSize;
            T **dataCopy = static_cast<T**>(malloc(sizeof(T*) * m_allocatedSize));
            // copy preceding items
            for (unsigned int i = 0; i < index; i++) {
                dataCopy[i] = m_data[i];
            }
            // assign the inserted item
            dataCopy[index] = item;
            // copy the suceeding items
            for (unsigned int i = index; i < m_size; i++) {
                dataCopy[i + 1] = m_data[i];
            }
            // reassign internals
            delete m_data;
            m_data = dataCopy;
            m_size++;
        }
    }

    void prepend(T *item) {
        insert(item, 0);
    }

    void remove(T *item) {
        int matchCount = 0;
        for (int i = 0; i < m_size; i++) {
            if (matchCount > 0) {
                m_data[i - matchCount] = m_data[i];
            }
            if (item == m_data[i]) {
                matchCount++;
            }
        }
        m_size =- matchCount;
    }

    void removeItemAt(unsigned int index) {
        if (index < (m_size - 1)) { // if not last item
            for (unsigned int i = index + 1; i < m_size; i++) {
                m_data[i - 1] = m_data[i];
            }
        }
        m_size--;
    }

    T* takeItemAt(unsigned int index) {
        T *item = itemAt(index);
        removeItemAt(index);
        return item;
    }

    T* lastItem() const {
        return m_data[m_size - 1];
    }

    void removeLastItem() {
        m_size--;
    }

    T* takeLastItem() {
        m_size--;
        return m_data[m_size];
    }

    void clear() {
        m_size = 0;
    }

    void map(void (*fn)(T *)) {
        for (unsigned int i = 0; i < m_size; i++) {
            (*fn)(m_data[i]);
        }
    }

    void map(void *ctx, void (*fn)(T *)) {
        for (unsigned int i = 0; i < m_size; i++) {
            (*fn)(ctx, m_data[i]);
        }
    }

private:
    void grow() {
        m_allocatedSize += m_chunkSize;
        m_data = static_cast<T**>(realloc(m_data, sizeof(T*) * m_allocatedSize));
    }

    unsigned int m_size;
    unsigned int m_allocatedSize;
    T **m_data;
    unsigned int m_chunkSize;
};

#endif // VFMDPOINTERARRAY_H
