#ifndef VFMDSCOPEDPOINTER_H
#define VFMDSCOPEDPOINTER_H

#include <assert.h>

template<typename T>
class VfmdScopedPointer
{
public:
    VfmdScopedPointer(T *p = 0) : m_pointer(p) { }
    ~VfmdScopedPointer() { delete m_pointer; }

    T* data() const { return m_pointer; }
    bool isNull() const {  return (m_pointer == 0); }
    void reset(T *p) { m_pointer = p; }
    T* take() { T *tmp = m_pointer; m_pointer = 0; return tmp; }
    inline T& operator*() const { assert(m_pointer != 0); return (*m_pointer); }
    inline T* operator->() const { assert(m_pointer != 0); return m_pointer; }
    void free() { delete m_pointer; m_pointer = 0; }

private:
    T *m_pointer;
};

#endif // VFMDSCOPEDPOINTER_H
