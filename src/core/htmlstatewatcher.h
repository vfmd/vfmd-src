#ifndef HTMLSTATEWATCHER_H
#define HTMLSTATEWATCHER_H

#include "vfmdline.h"
#include "vfmdpointerarray.h"

struct htmlparser_ctx_s;

class HtmlStateWatcher
{
public:
    enum State {
        TEXT_STATE,
        INDETERMINATE_STATE,             // Could be within a tag, or within a comment, or just text
        HTML_TAG_STATE,                  // Definitely within a tag
        HTML_COMMENT_STATE,              // Definitely within a comment
        INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE,         // Within a verbatim HTML element, well-formed or not
        CONTENT_OF_WELL_FORMED_VERBATIM_HTML_ELEMENT_STATE // Within a well-formed verbatim HTML element
    };

    HtmlStateWatcher();
    ~HtmlStateWatcher();

    // Add text to the parser.
    void addText(const VfmdByteArray &text);

    // The HTML state at the end of the last text that was added
    // while in normal mode (i.e. non-lookahead mode).
    State state() const;

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
        State state, lookaheadState;
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
