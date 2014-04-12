#ifndef VFMDBLOCKELEMENTHANDLER_H
#define VFMDBLOCKELEMENTHANDLER_H

#include "vfmdline.h"
#include "vfmdelementtreenode.h"
#include "vfmdinputlinesequence.h"
#include "vfmdconstants.h"
#include "core/vfmdpointerarray.h"

#define UNUSED_ARG(x) (void)x;

class VfmdBlockLineSequence;
class VfmdInputLineSequence;
class VfmdElementRegistry;

class VfmdBlockElementHandler {
public:
    VfmdBlockElementHandler() { }
    virtual void createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *currentLine, const VfmdLine *nextLine) const;
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
    virtual int elementType() const;
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const;
    virtual void endBlock();

    const VfmdInputLineSequence *parentLineSequence() const;
    const VfmdElementRegistry *registry() const;
    void setBlockParseTree(VfmdElementTreeNode *subtree);

    // linesSinceEndOfBlock():
    // This method is called after endBlock() to check if there
    // are any unprocessed lines.
    //
    // If there are lines passed as 'currentLine' to this
    // block's processBlockLine() that are not consumed in the block,
    // those lines should be returned, in the same order.
    //
    // Among the core vfmd blocks, the only block that uses this is
    // the paragraph block.
    virtual VfmdPointerArray<const VfmdLine> *linesSinceEndOfBlock();

private:
    /* Prevent copying of this class */
    VfmdBlockLineSequence(const VfmdBlockLineSequence& other);
    VfmdBlockLineSequence& operator=(const VfmdBlockLineSequence& other);

    VfmdInputLineSequence *m_parentLineSequence;
};

#endif // VFMDBLOCKELEMENTHANDLER_H
