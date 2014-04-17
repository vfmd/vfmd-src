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
    virtual ~VfmdBlockElementHandler() { }

    /* isStartOfBlock():
     * Should return true if the 'currentLine' is the start of the relevant block,
     * and false if not. The other arguments are:
     *   nextLine: the line upcoming after the currentLine
     *   containingBlockType: the type of the block that contains this block
     *   isAbuttingParagraph: true iff the previous line belongs to a paragraph and
     *                        is not a blank line.
     * If 'VfmdElementRegistry::setBlockCanAbutParagraph()' has not been called
     * on this block type, 'isAbuttingParagraph' is guaranteed to be false.
     * If 'isStartOfBlock()' returns true, it *might* be followed by
     * a call to 'createLineSequence()'.
     */
    virtual bool isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine,
                                int containingBlockType, bool isAbuttingParagraph);

    /* createLineSequence():
     * This shall be called only when 'isStartOfBlock()' returns true.
     * This method is expected to create a block line sequence object as a child
     * of 'parentLineSequence'.
     */
    virtual void createLineSequence(VfmdInputLineSequence *parentLineSequence) const;

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

    /* elementType():
     * For core-syntax blocks, this returns a VfmdConstants::VfmdBlockElementType type.
     * For your own blocks, please pick a number above VfmdConstants::USER_BLOCK_ELEMENT.
     */
    virtual int elementType() const;

    /* processBlockLine():
     * Process 'currentLine' as a member of this block.
     */
    virtual void processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine);

    /* isEndOfBlock():
     * If the block ends at 'currentLine', return true, else return false.
     */
    virtual bool isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine);

    /* endBlock():
     * This is called either when isEndOfBlock() returns true, or when there are
     * no more lines in the parent line sequence.
     * If this block should be part of the parse tree, this method should create an
     * appropriate tree node and call 'setBlockParseTree()'.
     */
    virtual void endBlock();

    const VfmdInputLineSequence *parentLineSequence() const;
    const VfmdElementRegistry *registry() const;
    void setBlockParseTree(VfmdElementTreeNode *subtree);

private:
    /* Prevent copying of this class */
    VfmdBlockLineSequence(const VfmdBlockLineSequence& other);
    VfmdBlockLineSequence& operator=(const VfmdBlockLineSequence& other);

    VfmdInputLineSequence *m_parentLineSequence;
};

#endif // VFMDBLOCKELEMENTHANDLER_H
