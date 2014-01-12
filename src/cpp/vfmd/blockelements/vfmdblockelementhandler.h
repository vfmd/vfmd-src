#ifndef VFMDBLOCKELEMENTHANDLER_H
#define VFMDBLOCKELEMENTHANDLER_H

#include "vfmdline.h"

#define UNUSED_PARAMETER(x) (void)x;

class VfmdBlockLineSequence;
class VfmdInputLineSequence;

class VfmdBlockElementHandler {
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

#endif // VFMDBLOCKELEMENTHANDLER_H
