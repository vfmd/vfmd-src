#ifndef VFMDSPANELEMENTSPROCESSOR_H
#define VFMDSPANELEMENTSPROCESSOR_H

#include "vfmdbytearray.h"
#include "vfmdspantagstack.h"

class VfmdElementRegistry;
class VfmdElementTreeNode;

class VfmdSpanElementsProcessor
{
public:
    VfmdSpanElementsProcessor(const VfmdByteArray &text, const VfmdElementRegistry *registry);
    VfmdElementTreeNode* parseTree() const;

private:
    VfmdSpanTagStack m_stack;
};

#endif // VFMDSPANELEMENTSPROCESSOR_H
