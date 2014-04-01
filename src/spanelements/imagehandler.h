#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "vfmdspanelementhandler.h"
#include "vfmdlinkrefmap.h"

class ImageHandler : public VfmdSpanElementHandler
{
public:
    ImageHandler(VfmdLinkRefMap *linkRefMap);
    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                          int currentPos,
                                          VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "image"; }

private:
    VfmdLinkRefMap *m_linkRefMap;
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
    void setLinkRefMap(VfmdLinkRefMap *linkRefMap);

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::IMAGE_ELEMENT; }
    virtual const char *elementTypeString() const { return "image"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;

private:
    ImageRefType m_linkRefType;
    VfmdByteArray m_ba1, m_ba2;
    VfmdByteArray m_altText;
    VfmdLinkRefMap *m_linkRefMap;
};

#endif // IMAGEHANDLER_H
