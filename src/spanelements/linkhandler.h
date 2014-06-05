#ifndef LINKHANDLER_H
#define LINKHANDLER_H

#include "vfmdspanelementhandler.h"
#include "core/vfmdlinkrefmap.h"

class LinkHandler : public VfmdSpanElementHandler
{
public:
    LinkHandler(const VfmdLinkRefMap *linkRefMap);
    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                                   int currentPos,
                                                   VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "link"; }

private:
    const VfmdLinkRefMap *m_linkRefMap;
};

class OpeningLinkTagStackNode : public VfmdOpeningSpanTagStackNode
{
public:
    OpeningLinkTagStackNode(const VfmdByteArray &text, int contentOffsetInText);

    virtual int type() const { return VfmdConstants::LINK_STACK_NODE; }
    virtual void populateEquivalentText(VfmdByteArray *ba) const;
    virtual void print() const;

    VfmdByteArray m_text;
    int m_contentOffsetInText;
};

class LinkTreeNode : public VfmdElementTreeNode {
public:
    enum LinkRefType {
        NO_REF,         // [text](url)
        CONTENT_AS_REF, // [ref]
        IMPLICIT_REF,   // [ref][]
        SEPARATE_REF    // [text][ref id]
    };

    LinkTreeNode(LinkRefType linkRefType, const VfmdByteArray& ba1, const VfmdByteArray& ba2);

    void setLinkRefMap(const VfmdLinkRefMap *linkRefMap);

    LinkRefType refType() const { return m_linkRefType; }
    const VfmdLinkRefMap *linkRefMap() const { return m_linkRefMap; }

    VfmdByteArray url() const { return (m_linkRefType == NO_REF? m_ba1 : VfmdByteArray()); }
    VfmdByteArray title() const { return (m_linkRefType == NO_REF? m_ba2 : VfmdByteArray()); }
    VfmdByteArray refIdText() const { return (m_linkRefType == NO_REF? VfmdByteArray() : m_ba1); }
    VfmdByteArray closeTagText() const { return (m_linkRefType == NO_REF? VfmdByteArray() : m_ba2); }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::LINK_ELEMENT; }
    virtual const char *elementTypeString() const { return "link"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
private:
    LinkRefType m_linkRefType;
    VfmdByteArray m_ba1, m_ba2;
    const VfmdLinkRefMap *m_linkRefMap;
};

#endif // LINKHANDLER_H
