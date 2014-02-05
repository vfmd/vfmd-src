#ifndef VFMDBLOCKELEMENTHANDLER_H
#define VFMDBLOCKELEMENTHANDLER_H

#include "vfmdline.h"
#include "vfmdelementtreenode.h"

#define UNUSED_ARG(x) (void)x;

class VfmdBlockLineSequence;
class VfmdInputLineSequence;
class VfmdElementRegistry;

class VfmdBlockElementHandler {
public:
    VfmdBlockElementHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~VfmdBlockElementHandler() { }

    /* A short text describing this syntax (eg. "emphasis", "strikethrough") */
    virtual const char *description() const;
};

class VfmdBlockLineSequence {
public:
    VfmdBlockLineSequence(const VfmdInputLineSequence *parentLineSequence);
    virtual ~VfmdBlockLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
    const VfmdInputLineSequence *parentLineSequence() const;
    const VfmdElementRegistry *registry() const;
private:
    const VfmdInputLineSequence *m_parentLineSequence;
};

#endif // VFMDBLOCKELEMENTHANDLER_H
