#ifndef AUTOLINKHANDLER_H
#define AUTOLINKHANDLER_H

#include "vfmdspanelementhandler.h"

class AutomaticLinkHandler : public VfmdSpanElementHandler
{
public:
    AutomaticLinkHandler();
    virtual int identifySpanTagStartingBetween(const VfmdByteArray &text,
                                               int fromPos, int toPos,
                                               VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "automatic-link"; }
};

class AutomaticLinkBracketedHandler : public VfmdSpanElementHandler
{
public:
    AutomaticLinkBracketedHandler();
    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                          int currentPos,
                                          VfmdSpanTagStack *stack) const;

    virtual const char *description() const { return "automatic-link <bracketed>"; }
};

class AutomaticLinkTreeNode : public VfmdElementTreeNode {
public:
    enum AutomaticUrlType {
        MAIL_URL_WITH_MAILTO,
        MAIL_URL_WITHOUT_MAILTO,
        OTHER_URL
    };

    AutomaticLinkTreeNode(AutomaticUrlType type, const VfmdByteArray& url) : m_type(type), m_url(url) { }

    AutomaticUrlType urlType() const { return m_type; }
    VfmdByteArray url() const { return m_url; }

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::AUTOMATIC_LINK_ELEMENT; }
    virtual const char *elementTypeString() const { return "automatic-link"; }

private:
    AutomaticUrlType m_type;
    VfmdByteArray m_url;
};

#endif // AUTOLINKHANDLER_H
