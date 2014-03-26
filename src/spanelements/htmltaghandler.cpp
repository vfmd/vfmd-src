#include "htmltaghandler.h"
#include "vfmddictionary.h"
#include "vfmdspantagstack.h"
#include "htmltextrenderer.h"

extern "C" {
#include "streamhtmlparser/htmlparser.h"
}

static void onIdentifyingStartTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::START_TAG;
        ctx->tagName = VfmdByteArray(tagName);
    }
}

static void onIdentifyingEndTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::END_TAG;
        ctx->tagName = VfmdByteArray(tagName);
    }
}

static void onIdentifyingEmptyTag(const char *tagName, void *context)
{
    HtmlTagHandler::ParserCallbackContext *ctx = static_cast<HtmlTagHandler::ParserCallbackContext *>(context);
    if (ctx->tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED) {
        ctx->tagType = HtmlTagHandler::ParserCallbackContext::EMPTY_TAG;
        ctx->tagName = VfmdByteArray(tagName);
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
{
}

void HtmlTagHandler::ParserCallbackContext::reset()
{
    tagType = HtmlTagHandler::ParserCallbackContext::UNDEFINED;
    tagName.clear();
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

            HtmlTreeNode* htmlTreeNode = new HtmlTreeNode(HtmlTreeNode::END_TAG_ONLY, callbackCtx->tagName,
                                                          text.mid(currentPos, offset - currentPos));
            stack->topNode()->appendToContainedElements(htmlTreeNode);
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
        callbackCtx->reset();
        return (offset - currentPos);

    }

    assert(tagType == HtmlTagHandler::ParserCallbackContext::NOT_HTML ||
           tagType == HtmlTagHandler::ParserCallbackContext::UNDEFINED);
    stack->topNode()->appendToContainedElements("<");
    callbackCtx->reset();
    return 1;
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

HtmlTreeNode::HtmlTreeNode(HtmlElementType type, const VfmdByteArray &tagName, const VfmdByteArray &html)
    : m_htmlElementType(type)
    , m_tagName(tagName)
    , m_html(html)
{
    assert(type != HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG);
}

HtmlTreeNode::HtmlTreeNode(HtmlElementType type, const VfmdByteArray &tagName, const VfmdByteArray& startTagHtml, const VfmdByteArray &endTagHtml)
    : m_htmlElementType(type)
    , m_tagName(tagName)
    , m_html(startTagHtml)
    , m_endTagHtml(endTagHtml)
{
    assert(type == HtmlTreeNode::START_TAG_WITH_MATCHING_END_TAG);
}

void HtmlTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice, VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        switch (m_htmlElementType) {
        case START_TAG_WITH_MATCHING_END_TAG:
            outputDevice->write(m_html);
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            outputDevice->write(m_endTagHtml);
            break;
        case EMPTY_TAG:
        case START_TAG_ONLY:
        case END_TAG_ONLY:
        case COMMENT:
            outputDevice->write(m_html);
            assert(hasChildren() == false);
            break;
        default:
            assert(false);
        }
    } else if (format == VfmdConstants::TREE_FORMAT) {
        renderTreePrefix(outputDevice, ancestorNodes, "+- span (raw-html) ");
        if (m_htmlElementType == COMMENT) {
            outputDevice->write("comment");
        } else {
            outputDevice->write(m_tagName);
        }
        if (m_htmlElementType == EMPTY_TAG) {
            outputDevice->write(", self-closing tag");
        } if (m_htmlElementType == START_TAG_ONLY) {
            outputDevice->write(", only start tag");
        } else if (m_htmlElementType == END_TAG_ONLY) {
            outputDevice->write(", only end tag");
        }
        outputDevice->write("\n");
        if (hasChildren()) {
            renderTreePrefix(outputDevice, ancestorNodes, (hasNext()? "|  |\n" : "   |\n"));
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
