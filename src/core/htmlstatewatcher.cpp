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

HtmlStateWatcher::HtmlStateWatcher()
    : m_htmlParserContext(0)
    , m_htmlParserLookaheadContext(0)
{
    m_callbackContext.isLookingAhead = false;
    m_callbackContext.state = HtmlStateWatcher::TEXT_STATE;
    m_callbackContext.lookaheadState = HtmlStateWatcher::INDETERMINATE_STATE;
    m_htmlParserContext = htmlparser_new();
    m_htmlParserContext->callback_context = &m_callbackContext;
    m_htmlParserContext->on_enter_possible_tag_or_comment = &onEnteringPossibleTagOrComment;
    m_htmlParserContext->on_exit_tag = &onIdentifyingAsTag;
    m_htmlParserContext->on_exit_comment = &onIdentifyingAsComment;
    m_htmlParserContext->on_cancel_possible_tag_or_comment = &onIdentifyingAsNotATagOrComment;
}

HtmlStateWatcher::~HtmlStateWatcher()
{
    if (m_htmlParserContext) {
        htmlparser_delete(m_htmlParserContext);
    }
    if (m_htmlParserLookaheadContext) {
        htmlparser_delete(m_htmlParserLookaheadContext);
    }
}

void HtmlStateWatcher::addText(const VfmdByteArray &text)
{
    if (m_callbackContext.isLookingAhead) {
        htmlparser_parse(m_htmlParserLookaheadContext, text.data(), text.size());
    } else {
        htmlparser_parse(m_htmlParserContext, text.data(), text.size());
    }
}

void HtmlStateWatcher::beginLookahead()
{
    m_callbackContext.isLookingAhead = true;
    m_htmlParserLookaheadContext = htmlparser_new();
    htmlparser_copy(m_htmlParserLookaheadContext, m_htmlParserContext);
}

void HtmlStateWatcher::endLookahead(bool consumeLookedaheadLines)
{
    m_callbackContext.isLookingAhead = false;
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
