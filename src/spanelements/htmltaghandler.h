#ifndef HTMLTAGHANDLER_H
#define HTMLTAGHANDLER_H

#include "vfmdspanelementhandler.h"
#include "vfmdbytearray.h"
#include "vfmdconstants.h"

struct htmlparser_ctx_s;
template<class T> class VfmdDictionary;
class HtmlTreeNode;

class HtmlTagHandler : public VfmdSpanElementHandler
{
public:
    HtmlTagHandler();
    ~HtmlTagHandler();

    virtual int identifySpanTagStartingAt(const VfmdByteArray &text,
                                          int currentPos,
                                          VfmdSpanTagStack *stack) const;
    virtual const char *description() const { return "raw-html"; }

    struct ParserCallbackContext {
        ParserCallbackContext();
        void reset();

        enum HtmlTagType {
            UNDEFINED,
            NOT_HTML,
            START_TAG,   // like <div>
            END_TAG,     // like </div>
            EMPTY_TAG,   // like <div/> or <br>
            COMMENT      // like <!-- -->
        };

        HtmlTagType tagType;
        VfmdByteArray tagName;
        bool isVerbatimHtmlStarterOrContainerTagEncountered;
        bool isNonPhrasingHtmlElementTagEncountered;
    };

private:
    ParserCallbackContext *htmlParserCallbackContext();
    htmlparser_ctx_s *htmlParserContext();

    ParserCallbackContext *m_callbackContext;
    htmlparser_ctx_s *m_htmlParserContext;
};

class OpeningHtmlTagStackNode : public VfmdOpeningSpanTagStackNode
{
public:
    OpeningHtmlTagStackNode(const VfmdByteArray& tagName, const VfmdByteArray& html);

    virtual int type() const { return VfmdConstants::RAW_HTML_STACK_NODE; }
    virtual void populateEquivalentText(VfmdByteArray *ba) const;
    virtual void print() const;

    HtmlTreeNode *toUnclosedStartHtmlTagTreeNode() const;

    const VfmdByteArray m_tagName;
    const VfmdByteArray m_html;
};

class HtmlTreeNode : public VfmdElementTreeNode {
public:
    enum HtmlNodeType {
        START_TAG_WITH_MATCHING_END_TAG,
        EMPTY_TAG,
        START_TAG_ONLY,
        END_TAG_ONLY,
        COMMENT,
        VERBATIM_HTML_CHUNK
    };

    HtmlTreeNode(HtmlNodeType htmlNodeType, const VfmdByteArray &tagName, const VfmdByteArray &startTagHtml, const VfmdByteArray& endTagHtml);
    HtmlTreeNode(HtmlNodeType htmlNodeType, const VfmdByteArray &tagName, const VfmdByteArray& html);
    HtmlTreeNode(HtmlNodeType htmlNodeType, const VfmdByteArray &verbatimHtml);

    HtmlNodeType htmlNodeType() const { return m_htmlNodeType; }
    VfmdByteArray tagName() const { return m_tagName; }
    VfmdByteArray startTagText() const;
    VfmdByteArray endTagText() const;
    VfmdByteArray fullHtmlText() const;

    // Reimplemented
    virtual ElementClassification elementClassification() const { return SPAN; }
    virtual int elementType() const { return VfmdConstants::HTML_ELEMENT; }
    virtual const char *elementTypeString() const { return "raw-html"; }

    virtual void renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const;
private:
    const HtmlNodeType m_htmlNodeType;
    const VfmdByteArray m_tagName, m_html, m_endTagHtml;
};

#endif // HTMLTAGHANDLER_H
