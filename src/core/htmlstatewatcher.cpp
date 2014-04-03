#include "htmlstatewatcher.h"
#include "core/vfmdutils.h"

extern "C" {
#include "streamhtmlparser/htmlparser.h"
}

static void onEnteringPossibleTagOrComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->tagState == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->tagState = HtmlStateWatcher::TEXT_STATE;
        }
        ctx->lookaheadTagState = HtmlStateWatcher::INDETERMINATE_STATE;
    } else {
        ctx->tagState = HtmlStateWatcher::INDETERMINATE_STATE;
    }
}

static void onIdentifyingAsTag(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->tagState == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->tagState = HtmlStateWatcher::HTML_TAG_STATE;
        }
        ctx->lookaheadTagState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->tagState = HtmlStateWatcher::TEXT_STATE;
    }
}

static void onIdentifyingAsComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->tagState == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->tagState = HtmlStateWatcher::HTML_COMMENT_STATE;
        }
        ctx->lookaheadTagState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->tagState = HtmlStateWatcher::TEXT_STATE;
    }
}

static void onIdentifyingAsNotATagOrComment(void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (ctx->isLookingAhead) {
        if (ctx->tagState == HtmlStateWatcher::INDETERMINATE_STATE) {
            ctx->tagState = HtmlStateWatcher::TEXT_STATE;
        }
        ctx->lookaheadTagState = HtmlStateWatcher::TEXT_STATE;
    } else {
        ctx->tagState = HtmlStateWatcher::TEXT_STATE;
    }
}

bool isVerbatimHtmlContainerTag(const char *tagName)
{
    return ((strcmp(tagName, "pre") == 0) ||
            (strcmp(tagName, "script") == 0) ||
            (strcmp(tagName, "style") == 0));
}

static bool isVerbatimHtmlStarterTag(const char *tagName)
{
    static const char *verbatimHtmlStarterElementsList[] = {
        "address", "article", "aside",  "blockquote", "details",
        "dialog",  "div",     "dl",     "fieldset",   "figure",
        "footer",  "form",    "header", "main",       "nav",
        "ol",      "section", "table",  "ul"
    };
    int i = indexOfStringInSortedList(tagName, verbatimHtmlStarterElementsList, 0, 19);
    return (i >= 0);
}

static void onIdentifyingStartTag(const char *tagName, void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (isVerbatimHtmlContainerTag(tagName)) {
        ctx->openVerbatimHtmlTagsStack->append(new VfmdByteArray(tagName));
        if (ctx->isLookingAhead) {
            ctx->lookaheadVerbatimContainerElementState = HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE;
        } else {
            ctx->verbatimContainerElementState = HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE;
        }
    } else if (isVerbatimHtmlStarterTag(tagName)) {
        if (ctx->isLookingAhead) {
            ctx->lookaheadVerbatimStarterElementState = HtmlStateWatcher::VERBATIM_STARTER_ELEMENT_SEEN;
        } else {
            ctx->verbatimStarterElementState = HtmlStateWatcher::VERBATIM_STARTER_ELEMENT_SEEN;
        }
    }
}

static void onIdentifyingEndTag(const char *tagName, void *context)
{
    HtmlStateWatcher::ParserCallbackContext *ctx = static_cast<HtmlStateWatcher::ParserCallbackContext *>(context);
    if (isVerbatimHtmlContainerTag(tagName)) {
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
            if (ctx->verbatimContainerElementState == HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE) {
                if (ctx->openVerbatimHtmlTagsStack->size() < ctx->numOfOpenVerbatimHtmlTagsAtStartOfLookahead) {
                    // The verbatim html tag that was opened just before the start of the lookahead
                    // has found its close tag. Therefore, that was a well-formed tag.
                    ctx->verbatimContainerElementState = HtmlStateWatcher::WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT;
                }
            }
            if (ctx->openVerbatimHtmlTagsStack->size() > 0) {
                ctx->lookaheadVerbatimContainerElementState = HtmlStateWatcher::INDETERMINATE_VERBATIM_CONTAINER_ELEMENT_STATE;
            } else {
                ctx->lookaheadVerbatimContainerElementState = HtmlStateWatcher::NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT;
            }
        } else {
            if (ctx->openVerbatimHtmlTagsStack->size() == 0) {
                ctx->verbatimContainerElementState = HtmlStateWatcher::NOT_WITHIN_WELL_FORMED_VERBATIM_CONTAINER_ELEMENT;
            }
        }
    } else if (isVerbatimHtmlStarterTag(tagName)) {
        if (ctx->isLookingAhead) {
            ctx->lookaheadVerbatimStarterElementState = HtmlStateWatcher::VERBATIM_STARTER_ELEMENT_SEEN;
        } else {
            ctx->verbatimStarterElementState = HtmlStateWatcher::VERBATIM_STARTER_ELEMENT_SEEN;
        }
    }
}

HtmlStateWatcher::HtmlStateWatcher()
    : m_htmlParserContext(0)
    , m_htmlParserLookaheadContext(0)
{
    m_callbackContext.isLookingAhead = false;
    m_callbackContext.tagState = HtmlStateWatcher::TEXT_STATE;
    m_callbackContext.verbatimContainerElementState = HtmlStateWatcher::NO_VERBATIM_CONTAINER_ELEMENT_SEEN;
    m_callbackContext.verbatimStarterElementState = HtmlStateWatcher::NO_VERBATIM_STARTER_ELEMENT_SEEN;
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
    m_callbackContext.lookaheadTagState = m_callbackContext.tagState;
    m_callbackContext.lookaheadVerbatimContainerElementState = m_callbackContext.verbatimContainerElementState;
    m_callbackContext.lookaheadVerbatimStarterElementState = m_callbackContext.verbatimStarterElementState;
}

void HtmlStateWatcher::endLookahead(bool consumeLookedaheadLines)
{
    m_callbackContext.isLookingAhead = false;
    m_callbackContext.numOfOpenVerbatimHtmlTagsAtStartOfLookahead = 0;
    if (consumeLookedaheadLines) {
        htmlparser_delete(m_htmlParserContext);
        m_htmlParserContext = m_htmlParserLookaheadContext;
        m_htmlParserLookaheadContext = 0;
        m_callbackContext.tagState = m_callbackContext.lookaheadTagState;
        m_callbackContext.verbatimContainerElementState = m_callbackContext.lookaheadVerbatimContainerElementState;
        m_callbackContext.verbatimStarterElementState = m_callbackContext.lookaheadVerbatimStarterElementState;
    }
}

HtmlStateWatcher::TagState HtmlStateWatcher::tagState() const
{
    return m_callbackContext.tagState;
}

HtmlStateWatcher::VerbatimContainerElementState HtmlStateWatcher::verbatimContainerElementState() const
{
    return m_callbackContext.verbatimContainerElementState;
}

HtmlStateWatcher::VerbatimStarterElementState HtmlStateWatcher::verbatimStarterElementState() const
{
    return m_callbackContext.verbatimStarterElementState;
}

bool HtmlStateWatcher::isLookingAhead() const
{
    return m_callbackContext.isLookingAhead;
}
