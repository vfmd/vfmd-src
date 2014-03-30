#include "htmlstatewatcher.h"

extern "C" {
#include "streamhtmlparser/htmlparser.h"
}

static void onEnteringPossibleTagOrComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->state == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->state = HtmlStateWatcher::TEXT_STATE;
        }
        ctx->lookaheadState = HtmlStateWatcher::INDETERMINATE_STATE;
    } else {
        ctx->state = HtmlStateWatcher::INDETERMINATE_STATE;
    }
}

static void onIdentifyingAsTag(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->state == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->state = HtmlStateWatcher::HTML_TAG_STATE;
        }
        ctx->lookaheadState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->state = HtmlStateWatcher::TEXT_STATE;
    }
}

static void onIdentifyingAsComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->state == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->state = HtmlStateWatcher::HTML_COMMENT_STATE;
        }
        ctx->lookaheadState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->state = HtmlStateWatcher::TEXT_STATE;
    }
}

static void onIdentifyingAsNotATagOrComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->state == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->state = HtmlStateWatcher::TEXT_STATE;
        }
        ctx->lookaheadState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->state = HtmlStateWatcher::TEXT_STATE;
    }
}

bool isVerbatimHtmlTag(const char *tagName)
{
    return ((strcmp(tagName, "pre") == 0) ||
            (strcmp(tagName, "script") == 0) ||
            (strcmp(tagName, "style") == 0));
}

static void onIdentifyingStartTag(const char *tagName, void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (isVerbatimHtmlTag(tagName)) {
        ctx->openVerbatimHtmlTagsStack->append(new VfmdByteArray(tagName));
        if (ctx->isLookingAhead) {
            // Nothing to do
        } else {
            ctx->state = HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE;
        }
    }
}

static void onIdentifyingEndTag(const char *tagName, void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (isVerbatimHtmlTag(tagName)) {
        int sz = (int) ctx->openVerbatimHtmlTagsStack->size();
        int topMostMatchIndex = -1;
        for (int i = sz - 1; i >= 0; i--) {
            if (ctx->openVerbatimHtmlTagsStack->itemAt(i)->isEqualTo(tagName)) {
                topMostMatchIndex = (int) i;
                break;
            }
        }
        if (topMostMatchIndex >= 0) {
            for (int i = sz - 1; i >= topMostMatchIndex; i--) {
                VfmdByteArray *ba = ctx->openVerbatimHtmlTagsStack->takeItemAt(i);
                delete ba;
            }
        }

        if (ctx->isLookingAhead) {
            if (ctx->state == HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE) {
                if (ctx->openVerbatimHtmlTagsStack->size() < ctx->numOfOpenVerbatimHtmlTagsAtStartOfLookahead) {
                    // The verbatim html tag that was opened just before the start of the lookahead
                    // has found its close tag. Therefore, that was a well-formed tag.
                    ctx->state = HtmlStateWatcher::CONTENT_OF_WELL_FORMED_VERBATIM_HTML_ELEMENT_STATE;
                }
            }
            if (ctx->openVerbatimHtmlTagsStack->size() > 0) {
                ctx->lookaheadState = HtmlStateWatcher::INDETERMINATE_VERBATIM_HTML_ELEMENT_STATE;
            } else {
                ctx->lookaheadState = HtmlStateWatcher::TEXT_STATE;
            }
        } else {
            if (ctx->openVerbatimHtmlTagsStack->size() == 0) {
                ctx->state = HtmlStateWatcher::TEXT_STATE;
            }
        }
    }
}

HtmlStateWatcher::HtmlStateWatcher()
    : m_htmlParserContext(0)
    , m_htmlParserLookaheadContext(0)
{
    m_callbackContext.isLookingAhead = false;
    m_callbackContext.state = HtmlStateWatcher::TEXT_STATE;
    m_callbackContext.lookaheadState = HtmlStateWatcher::INDETERMINATE_STATE;
    m_callbackContext.openVerbatimHtmlTagsStack = new VfmdPointerArray<VfmdByteArray>(16);
    m_callbackContext.numOfOpenVerbatimHtmlTagsAtStartOfLookahead = 0;

    m_htmlParserContext = htmlparser_new();
    m_htmlParserContext->callback_context = &m_callbackContext;
    m_htmlParserContext->on_enter_possible_tag_or_comment = &onEnteringPossibleTagOrComment;
    m_htmlParserContext->on_exit_tag = &onIdentifyingAsTag;
    m_htmlParserContext->on_exit_comment = &onIdentifyingAsComment;
    m_htmlParserContext->on_cancel_possible_tag_or_comment = &onIdentifyingAsNotATagOrComment;
    m_htmlParserContext->on_exit_start_tag = &onIdentifyingStartTag;
    m_htmlParserContext->on_exit_end_tag = &onIdentifyingEndTag;
}

HtmlStateWatcher::~HtmlStateWatcher()
{
    if (m_htmlParserContext) {
        htmlparser_delete(m_htmlParserContext);
    }
    if (m_htmlParserLookaheadContext) {
        htmlparser_delete(m_htmlParserLookaheadContext);
    }

    m_callbackContext.openVerbatimHtmlTagsStack->freeItemsAndClear();
    delete m_callbackContext.openVerbatimHtmlTagsStack;
}

void HtmlStateWatcher::addText(const VfmdByteArray &text, bool *isEndOfAddedTextWithinHtmlQuotedAttribute)
{
    if (m_callbackContext.isLookingAhead) {
        htmlparser_parse(m_htmlParserLookaheadContext, text.data(), text.size());
        if (isEndOfAddedTextWithinHtmlQuotedAttribute) {
            (*isEndOfAddedTextWithinHtmlQuotedAttribute) = htmlparser_is_attr_quoted(m_htmlParserLookaheadContext);
        }
    } else {
        htmlparser_parse(m_htmlParserContext, text.data(), text.size());
        if (isEndOfAddedTextWithinHtmlQuotedAttribute) {
            (*isEndOfAddedTextWithinHtmlQuotedAttribute) = htmlparser_is_attr_quoted(m_htmlParserContext);
        }
    }
}

void HtmlStateWatcher::beginLookahead()
{
    m_callbackContext.isLookingAhead = true;
    m_callbackContext.numOfOpenVerbatimHtmlTagsAtStartOfLookahead = m_callbackContext.openVerbatimHtmlTagsStack->size();
    m_htmlParserLookaheadContext = htmlparser_new();
    htmlparser_copy(m_htmlParserLookaheadContext, m_htmlParserContext);
}

void HtmlStateWatcher::endLookahead(bool consumeLookedaheadLines)
{
    m_callbackContext.isLookingAhead = false;
    m_callbackContext.numOfOpenVerbatimHtmlTagsAtStartOfLookahead = 0;
    m_callbackContext.openVerbatimHtmlTagsStack->freeItemsAndClear();
    if (consumeLookedaheadLines) {
        htmlparser_delete(m_htmlParserContext);
        m_htmlParserContext = m_htmlParserLookaheadContext;
        m_htmlParserLookaheadContext = 0;
        m_callbackContext.state = m_callbackContext.lookaheadState;
        m_callbackContext.lookaheadState = HtmlStateWatcher::INDETERMINATE_STATE;
    }
}

HtmlStateWatcher::State HtmlStateWatcher::state() const
{
    return m_callbackContext.state;
}

bool HtmlStateWatcher::isLookingAhead() const
{
    return m_callbackContext.isLookingAhead;
}
