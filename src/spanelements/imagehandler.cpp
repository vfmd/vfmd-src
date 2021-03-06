#include "imagehandler.h"
#include "vfmdspantagstack.h"
#include "vfmdregexp.h"
#include "core/vfmdlinkandimageutils.h"
#include "core/htmltextrenderer.h"

#include <stdio.h>

ImageHandler::ImageHandler(const VfmdLinkRefMap *linkRefMap)
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

void ImageTreeNode::setLinkRefMap(const VfmdLinkRefMap *linkRefMap)
{
    m_linkRefMap = linkRefMap;
}
