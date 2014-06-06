#include "htmltaghandler.h"
#include "vfmdspantagstack.h"
#include "core/htmltextrenderer.h"
#include "core/vfmdutils.h"

extern "C" {
#include "streamhtmlparser/htmlparser.h"
}

static bool isVoidHtmlElement(const char *tagName)
{
    static const char *voidHtmlElementsList[] = {
        "area", "base",  "br",     "col",    "embed",
        "hr",   "img",   "input",  "keygen", "link",
        "meta", "param", "source", "track",  "wbr"
    };
    int i = indexOfStringInSortedList(tagName, voidHtmlElementsList, 0, 15);
    return (i >= 0);
}

static bool isVerbatimHtmlStarterOrContainerTag(const char *tagName)
{
    static const char *verbatimHtmlStarterOrContainerElementsList[] = {
        "address", "article", "aside",  "blockquote", "details",
        "dialog",  "div",     "dl",     "fieldset",  "figure",
        "footer",  "form",    "header", "main",       "nav",
        "ol",      "pre",     "script", "section",    "style",
        "table",   "ul"
    };
    int i = indexOfStringInSortedList(tagName, verbatimHtmlStarterOrContainerElementsList, 0, 22);
    return (i >= 0);
}

static bool isPhrasingHtmlElementTag(const char *tagName)
{
    const char *phrasingHtmlElementsList[] = {
        "a",      "abbr",   "area",     "audio",    "b",
        "bdi",    "bdo",    "br",       "button",   "canvas",
        "cite",   "code",   "data",     "datalist", "del",
        "dfn",    "em",     "embed",    "i",        "iframe",
        "img",    "input",  "ins",      "kbd",      "keygen",
        "label",  "map",    "mark",     "meter",    "noscript",
        "object", "output", "progress", "q",        "ruby",
        "s",      "samp",   "select",   "small",    "span",
        "strong", "sub",    "sup",      "textarea", "time",
        "u",      "var",    "video",    "wbr"
    };
    int i = indexOfStringInSortedList(tagName, phrasingHtmlElementsList, 0, 49);
    return (i >= 0);
}

static void onIdentifyingStartTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        if (isVoidHtmlElement(tagName)) {
            ctx->tagType = HtmlTagHandler::ParserCallbackContext::EMPTY_TAG;
        } else {
            ctx->tagType = HtmlTagHandler::ParserCallbackContext::START_TAG;
        }
        ctx->tagName = VfmdByteArray(tagName);
        if (!ctx->isVerbatimHtmlStarterOrContainerTagEncountered && isVerbatimHtmlStarterOrContainerTag(tagName)) {
            ctx->isVerbatimHtmlStarterOrContainerTagEncountered = true;
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
        if (!ctx->isNonPhrasingHtmlElementTagEncountered && !isPhrasingHtmlElementTag(tagName)) {
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
    }
}

static void onIdentifyingEndTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::END_TAG;
        ctx->tagName = VfmdByteArray(tagName);
        if (!ctx->isVerbatimHtmlStarterOrContainerTagEncountered && isVerbatimHtmlStarterOrContainerTag(tagName)) {
            ctx->isVerbatimHtmlStarterOrContainerTagEncountered = true;
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
        if (!ctx->isNonPhrasingHtmlElementTagEncountered && !isPhrasingHtmlElementTag(tagName)) {
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
    }
}

static void onIdentifyingEmptyTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::EMPTY_TAG;
        ctx->tagName = VfmdByteArray(tagName);
        if (!ctx->isVerbatimHtmlStarterOrContainerTagEncountered && isVerbatimHtmlStarterOrContainerTag(tagName)) {
            ctx->isVerbatimHtmlStarterOrContainerTagEncountered = true;
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
        if (!ctx->isNonPhrasingHtmlElementTagEncountered && !isPhrasingHtmlElementTag(tagName)) {
            ctx->isNonPhrasingHtmlElementTagEncountered = true;
        }
    }
}

static void onIdentifyingAsComment(void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::COMMENT;
    }
}

static void onIdentifyingAsNotATagOrComment(void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::NOT_HTML;
    }
}

HtmlTagHandler::ParserCallbackContext::ParserCallbackContext()
    : tagType(HtmlTagHandler::ParserCallbackContext::UNDEFINED)
    , isVerbatimHtmlStarterOrContainerTagEncountered(false)
    , isNonPhrasingHtmlElementTagEncountered(false)
{
}

void HtmlTagHandler::ParserCallbackContext::reset()
{
    tagType = HtmlTagHandler::ParserCallbackContext::UNDEFINED;
    tagName.clear();
    isVerbatimHtmlStarterOrContainerTagEncountered = false;
    isNonPhrasingHtmlElementTagEncountered = false;
}

HtmlTagHandler::HtmlTagHandler()
    : m_callbackContext(0)
    , m_htmlParserContext(0)
{
}

HtmlTagHandler::~HtmlTagHandler()
{
    delete m_callbackContext;
    if (m_htmlParserContext) {
        htmlparser_delete(m_htmlParserContext);
    }
}

HtmlTagHandler::ParserCallbackContext *HtmlTagHandler::htmlParserCallbackContext()
{
    if (m_callbackContext == 0) {
        m_callbackContext = new HtmlTagHandler::ParserCallbackContext;
    }
    m_callbackContext->reset();
    return m_callbackContext;
}

htmlparser_ctx_s *HtmlTagHandler::htmlParserContext()
{
    if (m_htmlParserContext == 0) {
        m_htmlParserContext = htmlparser_new();
        m_htmlParserContext->callback_context = htmlParserCallbackContext();
        m_htmlParserContext->on_exit_start_tag = &onIdentifyingStartTag;
        m_htmlParserContext->on_exit_end_tag = &onIdentifyingEndTag;
        m_htmlParserContext->on_exit_empty_tag = &onIdentifyingEmptyTag;
        m_htmlParserContext->on_exit_comment = &onIdentifyingAsComment;
        m_htmlParserContext->on_cancel_possible_tag_or_comment = &onIdentifyingAsNotATagOrComment;
    }
    htmlparser_reset(m_htmlParserContext);
    return m_htmlParserContext;
}

int HtmlTagHandler::identifySpanTagStartingAt(const VfmdByteArray &text,
                                              int currentPos,
                                              VfmdSpanTagStack *stack) const
{
    assert(!text.isEscapedAtPosition(currentPos));
    assert(text.byteAt(currentPos) == '<');

    HtmlTagHandler::ParserCallbackContext *callbackCtx = const_cast<HtmlTagHandler *>(this)->htmlParserCallbackContext();
    htmlparser_ctx_s* htmlParserCtx = const_cast<HtmlTagHandler *>(this)->htmlParserContext();

    assert(callbackCtx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED);

    int sz = (int) text.size();
    int offset = currentPos;

    while ((offset < sz) &&
           (callbackCtx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED)) {
        int possibleEndingPos = text.indexOf('>', offset);
        if (possibleEndingPos < 0) { // No ending '>' found
            break;
        }
        htmlparser_parse(htmlParserCtx, text.data() + offset, possibleEndingPos - offset + 1);
        offset = possibleEndingPos + 1;
    }

    if (callbackCtx->isNonPhrasingHtmlElementTagEncountered) {
        // If we encountered a non-phrasing html tag (like "<td>"),
        // remove all non-html nodes in the stack
        stack->popNodesAboveIndexAsTextFragments(0, VfmdConstants::RAW_HTML_STACK_NODE /* excludeType */);
        stack->setNonPhrasingHtmlTagSeen(true);
    }

    if (callbackCtx->isVerbatimHtmlStarterOrContainerTagEncountered) {
        // If we encountered a verbatim-html-starter-tag (like "<div>")
        // or a verbatim-html-container-tag (like "<pre>"), then everything
        // that follows (including that tag) is verbatim-html.
        // We need not identify individual HTML tags within verbatim-html.
        HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::VERBATIM_HTML_CHUNK, text.mid(currentPos));
        stack->topNode()->appendToContainedElements(htmlTreeNode);
        callbackCtx->reset();
        return (text.size() - currentPos);
    }

    HtmlTagHandler::ParserCallbackContext::HtmlTagType tagType = callbackCtx->tagType;
    bool isTagNameCaptured = (callbackCtx->tagName.size() > 0);

    if (isTagNameCaptured) {
        if (tagType == HtmlTagHandler::ParserCallbackContext::EMPTY_TAG) {

            // Like "<tag/>"
            HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::EMPTY_TAG, callbackCtx->tagName,
                                                          text.mid(currentPos, offset - currentPos));
            stack->topNode()->appendToContainedElements(htmlTreeNode);
            callbackCtx->reset();
            return (offset - currentPos);

        } else if (tagType == HtmlTagHandler::ParserCallbackContext::START_TAG) {

            // Like "<tag>"
            stack->push(new OpeningHtmlTagStackNode(callbackCtx->tagName, text.mid(currentPos, offset - currentPos)));
            callbackCtx->reset();
            return (offset - currentPos);

        } else if (tagType == HtmlTagHandler::ParserCallbackContext::END_TAG) {

            // Like "</tag>"
            int topMostMatchingNodeIndex = stack->indexOfTopmostNodeOfType(VfmdConstants::RAW_HTML_STACK_NODE);
            if (topMostMatchingNodeIndex >= 0) {

                stack->popNodesAboveIndexAsTextFragments(topMostMatchingNodeIndex);
                OpeningHtmlTagStackNode *rawHtmlStackNode = dynamic_cast<OpeningHtmlTagStackNode *>(stack->topNode());
                assert(rawHtmlStackNode != 0);

                if (rawHtmlStackNode->m_tagName.isEqualTo(callbackCtx->tagName)) {
                    // What we have is a matching end tag
                    HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG,
                                                                  callbackCtx->tagName,
                                                                  rawHtmlStackNode->m_html,
                                                                  text.mid(currentPos, offset - currentPos));

                    htmlTreeNode->adoptContainedElements(rawHtmlStackNode);

                    VfmdOpeningSpanTagStackNode *poppedNode = stack->pop();
                    assert(poppedNode != 0);
                    assert(poppedNode->type() == VfmdConstants::RAW_HTML_STACK_NODE);
                    delete poppedNode;

                    stack->topNode()->appendToContainedElements(htmlTreeNode);
                    callbackCtx->reset();
                    return (offset - currentPos);
                }
            }

            // We have encountered a HTML tag mismatch
            // (end tag without a corresponding start tag)

            stack->popNodesAboveIndexAsTextFragments(0, VfmdConstants::RAW_HTML_STACK_NODE /* excludeType */);
            HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::END_TAG_ONLY, callbackCtx->tagName,
                                                          text.mid(currentPos, offset - currentPos));
            stack->topNode()->appendToContainedElements(htmlTreeNode);
            stack->setMismatchedHtmlTagSeen(true);
            callbackCtx->reset();
            return (offset - currentPos);

        } else {
            assert(tagType == HtmlTagHandler::ParserCallbackContext::COMMENT ||
                   tagType == HtmlTagHandler::ParserCallbackContext::NOT_HTML ||
                   tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED);
        }

    } // end of if (isTagNameCaptured)

    if (tagType == HtmlTagHandler::ParserCallbackContext::COMMENT) {

        // Like "<!-- Comment -->"
        HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::COMMENT, VfmdByteArray(),
                                                      text.mid(currentPos, offset - currentPos));
        stack->topNode()->appendToContainedElements(htmlTreeNode);
        stack->setHtmlCommentSeen(true);
        callbackCtx->reset();
        return (offset - currentPos);

    }

    assert(tagType == HtmlTagHandler::ParserCallbackContext::NOT_HTML ||
           tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED);
    return 0;
}

OpeningHtmlTagStackNode::OpeningHtmlTagStackNode(const VfmdByteArray &tagName, const VfmdByteArray &html)
    : m_tagName(tagName)
    , m_html(html)
{
}

void OpeningHtmlTagStackNode::populateEquivalentText(VfmdByteArray *ba) const
{
    assert(false);
    ba->clear();
    ba->append("[HTML]");
}

void OpeningHtmlTagStackNode::print() const
{
    printf("raw-html (");
    m_tagName.print();
    printf(" tag)");
}

HtmlTreeNode *OpeningHtmlTagStackNode::toUnclosedStartHtmlTagTreeNode() const
{
    return (new HtmlTreeNode(HtmlTreeNode::START_TAG_ONLY, m_tagName, m_html));
}

HtmlTreeNode::HtmlTreeNode(HtmlNodeType type, const VfmdByteArray &tagName, const VfmdByteArray &html)
    : m_htmlNodeType(type)
    , m_tagName(tagName)
    , m_html(html)
{
    assert(type != HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG);
}

HtmlTreeNode::HtmlTreeNode(HtmlNodeType type, const VfmdByteArray &tagName, const VfmdByteArray& startTagHtml, const VfmdByteArray &endTagHtml)
    : m_htmlNodeType(type)
    , m_tagName(tagName)
    , m_html(startTagHtml)
    , m_endTagHtml(endTagHtml)
{
    assert(type == HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG);
}

HtmlTreeNode::HtmlTreeNode(HtmlNodeType type, const VfmdByteArray &verbatimHtmlChunk)
    : m_htmlNodeType(type)
    , m_html(verbatimHtmlChunk)
{
    assert(type == HtmlTreeNode::VERBATIM_HTML_CHUNK);
}

VfmdByteArray HtmlTreeNode::startTagText() const
{
    if (m_htmlNodeType == START_TAG_ONLY ||
        m_htmlNodeType == START_TAG_WITH_MATCHING_END_TAG) {
        return m_html;
    }
    return VfmdByteArray();
}

VfmdByteArray HtmlTreeNode::endTagText() const
{
    if (m_htmlNodeType == START_TAG_WITH_MATCHING_END_TAG) {
        return m_endTagHtml;
    } else if (m_htmlNodeType == END_TAG_ONLY) {
        return m_html;
    }
    return VfmdByteArray();
}

VfmdByteArray HtmlTreeNode::fullHtmlText() const
{
    if (m_htmlNodeType == START_TAG_WITH_MATCHING_END_TAG) {
        // We don't have the enclosed html, so we cannot provide the full HTML text
        return VfmdByteArray();
    }
    return m_html;
}
