#include "imagehandler.h"
#include "vfmdspantagstack.h"
#include "vfmdregexp.h"
#include "core/vfmdlinkandimageutils.h"
#include "core/htmltextrenderer.h"

#include <stdio.h>

ImageHandler::ImageHandler(VfmdLinkRefMap *linkRefMap)
    : m_linkRefMap(linkRefMap)
{
}

static ImageTreeNode* handlePossibleSelfRefIdImageCloseTag(const VfmdByteArray &remainingText, int *consumedBytesCount)
{
    // Handles image close taglets of the form:
    // "][]"  (or)  "]"

    // Regexp that matches "][]"
    static VfmdRegexp reImageCloseEmptyRef("^(\\]\\s*\\[\\s*\\])");

    if (reImageCloseEmptyRef.matches(remainingText)) {
        // "][]"
        int matchingLength = reImageCloseEmptyRef.lengthOfMatch();
        VfmdByteArray closeTagText = reImageCloseEmptyRef.capturedText(0);
        (*consumedBytesCount) = matchingLength;
        return (new ImageTreeNode(ImageTreeNode::IMPLICIT_REF, VfmdByteArray(), closeTagText));
    }

    // "]"
    VfmdByteArray closeTagText = remainingText.mid(0, 1);
    assert(closeTagText.byteAt(0) == ']');
    assert(closeTagText.size() == 1);
    (*consumedBytesCount) = 1;
    return (new ImageTreeNode(ImageTreeNode::CONTENT_AS_REF, VfmdByteArray(), closeTagText));
}

int ImageHandler::identifySpanTagStartingAt(const VfmdByteArray &text,
                                            int currentPos,
                                            VfmdSpanTagStack *stack) const
{
    assert(text.byteAt(currentPos) == '!');
    assert(!text.isEscapedAtPosition(currentPos));

    if ((currentPos + 1) < text.size() && text.byteAt(currentPos + 1) == '[') {

        static VfmdRegexp reImageTagStarter("^!\\[(([^\\\\\\[\\]\\`]|\\\\.)*)\\]");

        if (reImageTagStarter.matches(text.mid(currentPos))) {

            int lengthOfImageTagStarter = reImageTagStarter.lengthOfMatch() - 1;
            VfmdByteArray altText = reImageTagStarter.capturedText(1);

            VfmdByteArray remainingText = text.mid(currentPos + lengthOfImageTagStarter);
            ImageTreeNode *imageNode = 0;
            int lengthOfImageTagEnd = 0;

            // Check for close tags of the form:
            // "][ref id]"
            if (imageNode == 0) {
                imageNode = handlePossibleRefIdCloseTag<ImageTreeNode>(remainingText, &lengthOfImageTagEnd);
            }

            // Check for close tags of the form:
            // "](url)"  (or)   "](url 'title')"  (or)   "](<url>)"  (or)  "](<url> 'title')"
            if (imageNode == 0) {
                imageNode = handlePossibleUrlTitleCloseTag<ImageTreeNode>(remainingText, &lengthOfImageTagEnd);
            }

            // Check for close tags of the form:
            // "][]"  (or)  "]"
            if (imageNode == 0) {
                imageNode = handlePossibleSelfRefIdImageCloseTag(remainingText, &lengthOfImageTagEnd);
            }

            assert(imageNode != 0);
            assert(lengthOfImageTagEnd > 0);

            imageNode->setAltText(altText);
            imageNode->setLinkRefMap(m_linkRefMap);

            stack->topNode()->appendToContainedElements(imageNode);
            return (lengthOfImageTagStarter + lengthOfImageTagEnd);

        }

        // First 2 bytes are "![", but the text doesn't match reImageTagStarter
        VfmdByteArray firstTwoBytesAsText = text.mid(currentPos, 2); // "!["
        stack->topNode()->appendToContainedElements(firstTwoBytesAsText);
        return 2;

    }

    // The second byte is not a '['
    return 0;
}

ImageTreeNode::ImageTreeNode(ImageRefType linkRefType, const VfmdByteArray& ba1, const VfmdByteArray& ba2)
    : m_linkRefType(linkRefType)
    , m_ba1(ba1)
    , m_ba2(ba2)
{
}

void ImageTreeNode::setAltText(const VfmdByteArray &altText)
{
    m_altText = altText;
}

void ImageTreeNode::setLinkRefMap(VfmdLinkRefMap *linkRefMap)
{
    m_linkRefMap = linkRefMap;
}

static void renderImageTag(const VfmdByteArray &url, const VfmdByteArray &title, const VfmdByteArray &altText, VfmdOutputDevice *outputDevice, int options)
{
    const bool shouldUseSelfClosingTags = ((options & HtmlTextRenderer::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS) == HtmlTextRenderer::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS);
    outputDevice->write("<img src=\"", 10);
    HtmlTextRenderer::render(url, outputDevice,
                             (HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                              HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                              HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
    if (altText.isValid()) {
        outputDevice->write("\" alt=\"", 7);
        HtmlTextRenderer::render(altText, outputDevice,
                                 (HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                  HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
    }
    if (title.isValid()) {
        outputDevice->write("\" title=\"", 9);
        HtmlTextRenderer::render(title, outputDevice,
                                 (HtmlTextRenderer::REMOVE_ESCAPING_BACKSLASHES |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                  HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
    }
    if (shouldUseSelfClosingTags) {
        outputDevice->write("\" />", 4);
    } else {
        outputDevice->write("\">", 2);
    }
}

void ImageTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                               VfmdOutputDevice *outputDevice,
                               VfmdElementTreeNodeStack *ancestorNodes) const
{
    assert(hasChildren() == false);
    if (format == VfmdConstants::HTML_FORMAT) {
        switch (m_linkRefType) {
        case NO_REF:

            // m_ba1 is linkUrl; m_ba2 is linkTitle
            renderImageTag(m_ba1, m_ba2, m_altText, outputDevice, renderOptions);
            break;

        case SEPARATE_REF:
        case IMPLICIT_REF:
        case CONTENT_AS_REF:

            // m_ba1 is refId / invalid; m_ba2 is closeTagText

            VfmdByteArray refId;
            if (m_linkRefType == SEPARATE_REF) {
                // m_ba1 is refId
                refId = m_ba1.simplified().toLowerCase();
            } else {
                // m_altText is refId
                assert(m_linkRefType == IMPLICIT_REF || m_linkRefType == CONTENT_AS_REF);
                refId = m_altText.simplified().toLowerCase();
            }
            if ((refId.size() > 0) && (m_linkRefMap->hasData(refId))) {
                // We found a definition for this ref id
                VfmdByteArray url = m_linkRefMap->linkUrl(refId);
                VfmdByteArray title = m_linkRefMap->linkTitle(refId);
                renderImageTag(url, title, m_altText, outputDevice, renderOptions);
            } else {
                // We didn't find a definition for this ref id
                outputDevice->write("![", 2);
                outputDevice->write(m_altText);
                outputDevice->write(m_ba2);
            }
            break;
        }
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
