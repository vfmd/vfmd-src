#ifndef VFMDSPANELEMENTSPROCESSOR_H
#define VFMDSPANELEMENTSPROCESSOR_H

class VfmdElementRegistry;
class VfmdLineArray;

class VfmdSpanElementsProcessor
{
public:
    static void processSpanElements(const VfmdLineArray *lineArray, const VfmdElementRegistry *registry);
};

#endif // VFMDSPANELEMENTSPROCESSOR_H
