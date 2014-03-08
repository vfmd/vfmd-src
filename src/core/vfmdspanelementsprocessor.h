#ifndef VFMDSPANELEMENTSPROCESSOR_H
#define VFMDSPANELEMENTSPROCESSOR_H

#include "vfmdbytearray.h"

class VfmdElementRegistry;
class VfmdElementTreeNode;

class VfmdSpanElementsProcessor
{
public:
    static VfmdElementTreeNode* processSpanElements(const VfmdByteArray &text, const VfmdElementRegistry *registry);
};

#endif // VFMDSPANELEMENTSPROCESSOR_H
