#ifndef HTMLSTATEWATCHER_H
#define HTMLSTATEWATCHER_H

#include "vfmdline.h"

struct htmlparser_ctx_s;

class HtmlStateWatcher
{
public:
    enum State {
        INDETERMINATE_STATE,
        TEXT_STATE,
        HTML_TAG_STATE,
        HTML_COMMENT_STATE
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
    };

private:
    ParserCallbackContext m_callbackContext;
    htmlparser_ctx_s *m_htmlParserContext;
    htmlparser_ctx_s *m_htmlParserLookaheadContext;
};

#endif // HTMLSTATEWATCHER_H
