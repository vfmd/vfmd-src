#ifndef VFMDBLOCKELEMENT_H
#define VFMDBLOCKELEMENT_H

#include "vfmdline.h"

#define UNUSED_PARAMETER(x) (void)x;

class VfmdBlockLineSequence;
class VfmdInputLineSequence;

class VfmdBlockSyntaxHandler {
public:
    virtual VfmdBlockLineSequence *createBlockLineSequence(const VfmdInputLineSequence *lineSequence);
};

class VfmdBlockLineSequence {
public:
    VfmdBlockLineSequence(const VfmdInputLineSequence *parentLineSequence);
    virtual ~VfmdBlockLineSequence();
    virtual void processLine(const VfmdLine &currentLine, const VfmdLine &nextLine);
    virtual bool isAtEnd() const;
};

#endif // VFMDBLOCKELEMENT_H
