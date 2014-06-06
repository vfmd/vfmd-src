#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "vfmdspanelementhandler.h"
#include "core/vfmdlinkrefmap.h"

class ImageHandler : public VfmdSpanElementHandler
{
public:
    ImageHandler(const VfmdLinkRefMap *linkRefMap);
    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                          int currentPos,
                                          VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "image"; }

private:
    const VfmdLinkRefMap *m_linkRefMap;
};

class ImageTreeNode : public VfmdElementTreeNode {
public:
    enum ImageRefType {
        NO_REF,         // [text](url)
        CONTENT_AS_REF, // [ref]
        IMPLICIT_REF,   // [ref][]
        SEPARATE_REF    // [text][ref id]
    };

    ImageTreeNode(ImageRefType linkRefType, const VfmdByteArray& ba1, const VfmdByteArray& ba2);

    void setAltText(const VfmdByteArray &altText);
    void setLinkRefMap(const VfmdLinkRefMap *linkRefMap);

    ImageRefType refType() const { return m_linkRefType; }
    VfmdByteArray altText() const { return m_altText; }
    const VfmdLinkRefMap *linkRefMap() const { return m_linkRefMap; }

    VfmdByteArray url() const { return (m_linkRefType == NO_REF? m_ba1 : VfmdByteArray()); }
    VfmdByteArray title() const { return (m_linkRefType == NO_REF? m_ba2 : VfmdByteArray()); }
    VfmdByteArray refIdText() const { return (m_linkRefType == NO_REF?
                                                  VfmdByteArray() :
                                                  (m_linkRefType == SEPARATE_REF? m_ba1 : m_altText)); }
    VfmdByteArray closeTagText() const { return (m_linkRefType == NO_REF? VfmdByteArray() : m_ba2 ); }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::IMAGE_ELEMENT; }
    virtual const char *elementTypeString() const { return "image"; }

private:
    ImageRefType m_linkRefType;
    VfmdByteArray m_ba1, m_ba2;
    VfmdByteArray m_altText;
    const VfmdLinkRefMap *m_linkRefMap;
};

#endif // IMAGEHANDLER_H
