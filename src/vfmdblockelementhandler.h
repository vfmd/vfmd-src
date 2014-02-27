#ifndef VFMDBLOCKELEMENTHANDLER_H
#define VFMDBLOCKELEMENTHANDLER_H

#include "vfmdline.h"
#include "vfmdelementtreenode.h"
#include "vfmdinputlinesequence.h"

#define UNUSED_ARG(x) (void)x;

class VfmdBlockLineSequence;
class VfmdInputLineSequence;
class VfmdElementRegistry;

class VfmdBlockElementHandler {
public:
    VfmdBlockElementHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence);
    virtual ~VfmdBlockElementHandler() { }

    /* A short text describing this syntax (eg. "paragraph", "code-block") */
    virtual const char *description() const;

private:
    /* Prevent copying of this class */
    VfmdBlockElementHandler(const VfmdBlockElementHandler& other);
    VfmdBlockElementHandler& operator=(const VfmdBlockElementHandler& other);
};

class VfmdBlockLineSequence {
public:
    VfmdBlockLineSequence(const VfmdInputLineSequence *parentLineSequence);
    virtual ~VfmdBlockLineSequence();
    virtual void processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence);
    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const;
    virtual VfmdElementTreeNode* endBlock();
    const VfmdInputLineSequence *parentLineSequence() const;
    const VfmdElementRegistry *registry() const;
private:
    /* Prevent copying of this class */
    VfmdBlockLineSequence(const VfmdBlockLineSequence& other);
    VfmdBlockLineSequence& operator=(const VfmdBlockLineSequence& other);

    const VfmdInputLineSequence *m_parentLineSequence;
};

#endif // VFMDBLOCKELEMENTHANDLER_H
