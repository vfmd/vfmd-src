#ifndef HTMLSTATEWATCHER_H
#define HTMLSTATEWATCHER_H

#include "vfmdline.h"
#include "vfmdpointerarray.h"

struct htmlparser_ctx_s;

class HtmlStateWatcher
{
public:
    enum TagState {
        TEXT_STATE,
        INDETERMINATE_STATE,             // Could be within a tag, or within a comment, or just text
        HTML_TAG_STATE,                  // Definitely within a tag
        HTML_COMMENT_STATE               // Definitely within a comment
    };

    enum VerbatimContainerElementState {
        NO_VERBATIM_CONTAINER_ELEMENT_SEEN,
        INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE,   // Within a verbatim HTML element, well-formed or not
        WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT,    // Within a well-formed verbatim HTML element
        NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT // Either after the close of a well-formed verbatim HTML element
                                                          // or within a not-well-formed verbatim HTML element
    };

    enum VerbatimStarterElementState {
        NO_VERBATIM_STARTER_ELEMENT_SEEN,
        VERBATIM_STARTER_ELEMENT_SEEN
    };

    HtmlStateWatcher();
    ~HtmlStateWatcher();

    // Add text to the parser.
    void addText(const VfmdByteArray &text, bool *isEndOfAddedTextWithinHtmlQuotedAttribute = 0);

    // The HTML state at the end of the last text that was added
    // while in normal mode (i.e. non-lookahead mode).
    TagState tagState() const;
    VerbatimContainerElementState verbatimContainerElementState() const;
    VerbatimStarterElementState verbatimStarterElementState() const;

    // If the state() is indeterminate, you can begin a lookahead.
    // When in lookahead mode, the added text is used as lookahead
    // text i.e. the text is only used to figure out the HTML state
    // at the end of the last added non-lookahead text.
    void beginLookahead();

    // End an ongoing lookahead.
    void endLookahead(bool consumeLookedaheadLines);

    // Reset the parser
    void reset();

    // Are we in lookahead mode?
    bool isLookingAhead() const;

    struct ParserCallbackContext {
        TagState tagState, lookaheadTagState;
        VerbatimContainerElementState verbatimContainerElementState, lookaheadVerbatimContainerElementState;
        VerbatimStarterElementState verbatimStarterElementState, lookaheadVerbatimStarterElementState;
        bool isLookingAhead;
        VfmdPointerArray<VfmdByteArray> *openVerbatimHtmlTagsStack;
        int numOfOpenVerbatimHtmlTagsAtStartOfLookahead;
    };

private:
    ParserCallbackContext m_callbackContext;
    htmlparser_ctx_s *m_htmlParserContext;
    htmlparser_ctx_s *m_htmlParserLookaheadContext;
};

#endif // HTMLSTATEWATCHER_H
