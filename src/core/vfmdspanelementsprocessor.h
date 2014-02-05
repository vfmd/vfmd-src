#ifndef VFMDSPANELEMENTSPROCESSOR_H
#define VFMDSPANELEMENTSPROCESSOR_H

class VfmdElementRegistry;
class VfmdLineArray;
class VfmdElementTreeNode;

class VfmdSpanElementsProcessor
{
public:
    static VfmdElementTreeNode* processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry);
};

#endif // VFMDSPANELEMENTSPROCESSOR_H
