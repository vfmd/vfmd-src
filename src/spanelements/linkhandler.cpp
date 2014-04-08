#include "linkhandler.h"
#include "vfmdspantagstack.h"
#include "core/vfmdlinkandimageutils.h"
#include "core/htmltextrenderer.h"

LinkHandler::LinkHandler(VfmdLinkRefMap *linkRefMap)
    : m_linkRefMap(linkRefMap)
{
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
                linkNode = handlePossibleRefIdCloseTag<LinkTreeNode>(remainingText, &consumedBytesCount);
            }

            // Check for link close tags of the form:
            // "](url)"  (or)   "](url 'title')"  (or)   "](<url>)"  (or)  "](<url> 'title')"
            if (linkNode == 0) {
                linkNode = handlePossibleUrlTitleCloseTag<LinkTreeNode>(remainingText, &consumedBytesCount);
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
    HtmlTextRenderer::renderURL(outputDevice, url);
    if (title.isValid()) {
        outputDevice->write("\" title=\"", 9);
        HtmlTextRenderer::renderTagAttribute(outputDevice, title);
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
