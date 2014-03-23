#include "linkhandler.h"
#include "vfmdspantagstack.h"
#include "vfmdcommonregexps.h"
#include "htmltextrenderer.h"

LinkHandler::LinkHandler(VfmdLinkRefMap *linkRefMap)
    : m_linkRefMap(linkRefMap)
{
}

static VfmdByteArray enclosedStringOfQuotedString(const VfmdByteArray &quotedString)
{
    char firstByte = quotedString.byteAt(0);
    if (firstByte == '\'') {
        VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithSingleQuotedString();
        if (reQuotedString.matches(quotedString) &&
            (reQuotedString.lengthOfMatch() == quotedString.size())) {
            return reQuotedString.capturedText(1);
        }
    } else if (firstByte == '\"') {
        VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithDoubleQuotedString();
        if (reQuotedString.matches(quotedString) &&
            (reQuotedString.lengthOfMatch() == quotedString.size())) {
            return reQuotedString.capturedText(1);
        }
    }
    return VfmdByteArray();
}

static LinkTreeNode* handlePossibleRefIdLinkCloseTag(const VfmdByteArray &remainingText, int *consumedBytesCount)
{
    // Handles link close tags of the form:
    // "][ref id]"

    // Regexp that matches "] [ref id]"
    static VfmdRegexp reLinkCloseWithRefId("^\\]\\s*\\[(([^\\\\\\[\\]\\`]|\\\\.)+)\\]");

    if (reLinkCloseWithRefId.matches(remainingText)) {
        int matchingLength = reLinkCloseWithRefId.lengthOfMatch();
        VfmdByteArray refIdText = reLinkCloseWithRefId.capturedText(1);
        VfmdByteArray closeTagText = reLinkCloseWithRefId.capturedText(0);
        (*consumedBytesCount) = matchingLength;
        return (new LinkTreeNode(LinkTreeNode::SEPARATE_REF, refIdText, closeTagText));
    }
    return 0;
}

static LinkTreeNode* handlePossibleUrlTitleLinkCloseTag(const VfmdByteArray &remainingText, int *consumedBytesCount)
{
    // Handles link close tags of the form:
    // "](url)"  (or)   "](url 'title')"  (or)   "](<url>)"  (or)  "](<url> 'title')"

    VfmdByteArray rawUrlString;
    int lengthOfLinkCloseStart = 0;

    // Regexp that matches "] (url"
    static VfmdRegexp reLinkCloseStartWithURL("^\\]\\s*\\(\\s*([^\\(\\)<>\\`\\s]+)");

    if (reLinkCloseStartWithURL.matches(remainingText)) {
        lengthOfLinkCloseStart = reLinkCloseStartWithURL.lengthOfMatch();
        rawUrlString = reLinkCloseStartWithURL.capturedText(1);
    } else {

        // Regexp that matches "] (<url>"
        static VfmdRegexp reLinkCloseStartWithBracketedURL("^\\]\\s*\\(\\s*<([^<>\\`]*)>");

        if (reLinkCloseStartWithBracketedURL.matches(remainingText) == 0) {
            lengthOfLinkCloseStart = reLinkCloseStartWithBracketedURL.lengthOfMatch();
            rawUrlString = reLinkCloseStartWithBracketedURL.capturedText(1);
        }
    }

    if (lengthOfLinkCloseStart > 0) {

        VfmdByteArray residualString = remainingText.mid(lengthOfLinkCloseStart);
        VfmdByteArray titleString;
        int lengthOfLinkCloseEnd = 0;

        // Regexp that matches ")"
        static VfmdRegexp reLinkCloseEndWithoutTitle("^\\s*\\)");

        if (reLinkCloseEndWithoutTitle.matches(residualString)) {
            lengthOfLinkCloseEnd = reLinkCloseEndWithoutTitle.lengthOfMatch();
        } else {

            // Regexp that matches "'title')"
            static VfmdRegexp reLinkCloseEndWitTitle("^\\s*(\"(([^\\\\\"\\`]|\\\\.)*)\"|'(([^\\\\'\\`]|\\\\.)*)')\\s*\\)");

            if (reLinkCloseEndWitTitle.matches(residualString)) {
                lengthOfLinkCloseEnd = reLinkCloseEndWitTitle.lengthOfMatch();
                VfmdByteArray attributesString = reLinkCloseEndWitTitle.capturedText(1);
                assert(attributesString.size() > 0);
                titleString = enclosedStringOfQuotedString(attributesString);
            }

        }

        if (lengthOfLinkCloseEnd > 0) {
            VfmdByteArray linkUrl = rawUrlString.bytesInStringRemoved(" \n\r\f"); // Space, LF, CR and FF
            VfmdByteArray linkTitle = titleString.bytesInStringRemoved("\n");     // LF only
            (*consumedBytesCount) = (lengthOfLinkCloseStart + lengthOfLinkCloseEnd);
            return (new LinkTreeNode(LinkTreeNode::NO_REF, linkUrl, linkTitle));
        }
    }

    return 0;
}

static LinkTreeNode* handlePossibleSelfRefIdLinkCloseTag(const VfmdByteArray &text, int currentPos, int *consumedBytesCount, const OpeningLinkTagStackNode *openingLinkStackNode)
{
    // Handles link close tags of the form:
    // "][]"  (or)  "]"

    VfmdByteArray refIdText = text.mid(openingLinkStackNode->m_contentOffsetInText, currentPos - openingLinkStackNode->m_contentOffsetInText);

    // Regexp that matches "][]"
    static VfmdRegexp reLinkCloseEmptyRef("^(\\]\\s*\\[\\s*\\])");

    if (reLinkCloseEmptyRef.matches(text.mid(currentPos))) {
        // "][]"
        int matchingLength = reLinkCloseEmptyRef.lengthOfMatch();
        assert(currentPos > 0);
        VfmdByteArray closeTagText = reLinkCloseEmptyRef.capturedText(0);
        (*consumedBytesCount) = matchingLength;
        return (new LinkTreeNode(LinkTreeNode::IMPLICIT_REF, refIdText, closeTagText));
    }

    // "]"
    VfmdByteArray closeTagText = text.mid(currentPos, 1);
    assert(closeTagText.byteAt(0) == ']');
    assert(closeTagText.size() == 1);
    (*consumedBytesCount) = 1;
    return (new LinkTreeNode(LinkTreeNode::CONTENT_AS_REF, refIdText, closeTagText));
}

int LinkHandler::identifySpanTagStartingAt(const VfmdByteArray &text, int currentPos, VfmdSpanTagStack *stack) const
{
    char currentByte = text.byteAt(currentPos);

    assert(!text.isEscapedAtPosition(currentPos));
    assert(currentByte == '[' || currentByte == ']');

    if (currentByte == '[') {

        // Potential opening link tag
        stack->push(new OpeningLinkTagStackNode(text, currentPos + 1));
        return 1;

    } else if (currentByte == ']') {

        // Potential closing link tag
        int topMostMatchingNodeIndex = stack->indexOfTopmostNodeOfType(VfmdConstants::LINK_STACK_NODE);
        if (topMostMatchingNodeIndex >= 0) {

            stack->popNodesAboveIndexAsTextFragments(topMostMatchingNodeIndex);
            OpeningLinkTagStackNode *linkStackNode = dynamic_cast<OpeningLinkTagStackNode *>(stack->topNode());
            assert(linkStackNode != 0);

            VfmdByteArray remainingText = text.mid(currentPos);
            LinkTreeNode *linkNode = 0;
            int consumedBytesCount = 0;

            // Check for link close tags of the form:
            // "][ref id]"
            if (linkNode == 0) {
                linkNode = handlePossibleRefIdLinkCloseTag(remainingText, &consumedBytesCount);
            }

            // Check for link close tags of the form:
            // "](url)"  (or)   "](url 'title')"  (or)   "](<url>)"  (or)  "](<url> 'title')"
            if (linkNode == 0) {
                linkNode = handlePossibleUrlTitleLinkCloseTag(remainingText, &consumedBytesCount);
            }

            // Check for link close tags of the form:
            // "][]"  (or)  "]"
            if (linkNode == 0) {
                linkNode = handlePossibleSelfRefIdLinkCloseTag(text, currentPos, &consumedBytesCount, linkStackNode);
            }

            assert(linkNode != 0);
            assert(consumedBytesCount > 0);

            linkNode->adoptContainedElements(linkStackNode);
            linkNode->setLinkRefMap(m_linkRefMap);

            VfmdOpeningSpanTagStackNode *poppedNode = stack->pop();
            assert(poppedNode != 0);
            assert(poppedNode->type() == VfmdConstants::LINK_STACK_NODE);
            delete poppedNode;

            stack->removeNodesOfTypeAsTextFragments(VfmdConstants::LINK_STACK_NODE);
            stack->topNode()->appendToContainedElements(linkNode);

            return consumedBytesCount;

        } // end of if (topMostMatchingNodeIndex >= 0)
    } // end of if (currentByte == ']')

    // Not a link
    return 0;
}

OpeningLinkTagStackNode::OpeningLinkTagStackNode(const VfmdByteArray &text, int contentOffsetInText)
    : m_text(text)
    , m_contentOffsetInText(contentOffsetInText)
{
}

void OpeningLinkTagStackNode::populateEquivalentText(VfmdByteArray *ba) const
{
    ba->clear();
    ba->appendByte('[');
}

void OpeningLinkTagStackNode::print() const
{
    printf("link");
}

LinkTreeNode::LinkTreeNode(LinkRefType linkRefType, const VfmdByteArray& ba1, const VfmdByteArray& ba2)
    : m_linkRefType(linkRefType)
    , m_ba1(ba1)
    , m_ba2(ba2)
{
}

void LinkTreeNode::setLinkRefMap(VfmdLinkRefMap *linkRefMap)
{
    m_linkRefMap = linkRefMap;
}

static void renderLinkOpenTag(const VfmdByteArray &url, const VfmdByteArray &title, VfmdOutputDevice *outputDevice)
{
    outputDevice->write("<a href=\"", 9);
    HtmlTextRenderer::render(url, outputDevice,
                             (HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                              HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                              HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
    if (title.isValid()) {
        outputDevice->write("\" title=\"", 9);
        HtmlTextRenderer::render(title, outputDevice,
                                 (HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                  HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
    }
    outputDevice->write("\">", 2);
}

void LinkTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                            VfmdOutputDevice *outputDevice,
                            VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        switch (m_linkRefType) {
        case NO_REF:

            // m_ba1 is linkUrl; m_ba2 is linkTitle
            renderLinkOpenTag(m_ba1, m_ba2, outputDevice);
            renderChildren(format, renderOptions, outputDevice, ancestorNodes);
            outputDevice->write("</a>", 4);
            break;

        case SEPARATE_REF:
        case IMPLICIT_REF:
        case CONTENT_AS_REF:

            // m_ba1 is refIdText; m_ba2 is closeTagText
            VfmdByteArray refId = m_ba1.simplified().toLowerCase();
            if ((refId.size() > 0) && (m_linkRefMap->hasData(refId))) {
                // We found a definition for this ref id
                VfmdByteArray linkUrl = m_linkRefMap->linkUrl(refId);
                VfmdByteArray linkTitle = m_linkRefMap->linkTitle(refId);
                renderLinkOpenTag(linkUrl, linkTitle, outputDevice);
                renderChildren(format, renderOptions, outputDevice, ancestorNodes);
                outputDevice->write("</a>", 4);
            } else {
                // We didn't find a definition for this ref id
                outputDevice->write('[');
                renderChildren(format, renderOptions, outputDevice, ancestorNodes);
                outputDevice->write(m_ba2);
            }
            break;
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
