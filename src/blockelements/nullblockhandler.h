#ifndef NULLBLOCKHANDLER_H
#define NULLBLOCKHANDLER_H

#include "vfmdblockelementhandler.h"

class NullBlockLineSequence : public VfmdBlockLineSequence {
public:
    NullBlockLineSequence(const VfmdInputLineSequence *parent)
        : VfmdBlockLineSequence(parent) { }

    virtual ~NullBlockLineSequence() { }

    virtual void processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence) {
        UNUSED_ARG(currentLine);
        UNUSED_ARG(isEndOfParentLineSequence);
    }

    virtual bool isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const {
        return (!nextLine.isValid() || !nextLine.isBlankLine());
    }

    virtual VfmdElementTreeNode* endBlock() {
        return 0;
    }
};

class NullBlockHandler : public VfmdBlockElementHandler
{
public:
    NullBlockHandler() { }

    virtual void createChildSequence(VfmdInputLineSequence *lineSequence) {
        if (lineSequence->currentLine().isBlankLine()) {
            NullBlockLineSequence *nullBlockLineSequence = new NullBlockLineSequence(lineSequence);
            lineSequence->setChildSequence(nullBlockLineSequence);
        }
    }

    virtual ~NullBlockHandler() { }

    virtual const char *description() const { return "null-block"; }
};

#endif // NULLBLOCKHANDLER_H
