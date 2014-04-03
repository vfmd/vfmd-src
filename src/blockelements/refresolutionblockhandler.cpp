#include "refresolutionblockhandler.h"
#include "core/vfmdcommonregexps.h"

RefResolutionBlockHandler::RefResolutionBlockHandler(VfmdLinkRefMap *linkRefMap)
    : m_linkRefMap(linkRefMap)
{
}

void RefResolutionBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine *firstLine, const VfmdLine *nextLine) const
{
    if (firstLine->leadingSpacesCount() >= 4) {
        return;
    }

    if (firstLine->firstNonSpace() != '[') {
        return;
    }

    bool isRefResolutionBlock = false;
    VfmdByteArray firstLineText = firstLine->content();
    VfmdByteArray firstLineTrailingText;
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    if (reLabelAndPlainURL.matches(firstLineText)) {
        firstLineTrailingText = reLabelAndPlainURL.capturedText(2);
        isRefResolutionBlock = true;
    } else {
        VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
        if (reLabelAndBracketedURL.matches(firstLineText)) {
            firstLineTrailingText = reLabelAndBracketedURL.capturedText(2);
            isRefResolutionBlock = true;
        }
    }

    if (isRefResolutionBlock) {
        int numOfLinesInSequence = 1;
        VfmdByteArray linkDefText = firstLineText;
        if ((nextLine != 0) &&                                     // There exists a next line, and
            (firstLineTrailingText.indexOfFirstNonSpace() < 0)) {  // The first line does not have trailing non-space chars
            // The next line is also part of the ref-resolution block
            VfmdByteArray secondLineText = nextLine->content();
            VfmdRegexp reTitleLine = VfmdCommonRegexps::refResolutionBlockTitleLine();
            if (reTitleLine.matches(secondLineText)) {
                numOfLinesInSequence = 2;
                linkDefText.append(secondLineText);
            }
        }
        lineSequence->setChildSequence(new RefResolutionBlockLineSequence(lineSequence, numOfLinesInSequence, linkDefText, m_linkRefMap));
    }
}

RefResolutionBlockLineSequence::RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                                               int numOfLines,
                                                               const VfmdByteArray &linkDefinitionText,
                                                               VfmdLinkRefMap *linkRefMap)
    : VfmdBlockLineSequence(parent)
    , m_numOfLinesSeen(0)
    , m_numOfLinesInSequence(numOfLines)
    , m_linkDefText(linkDefinitionText)
    , m_linkRefMap(linkRefMap)
{
    assert(numOfLines == 1 || numOfLines == 2);
}

void RefResolutionBlockLineSequence::processBlockLine(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    m_numOfLinesSeen++;
}

bool RefResolutionBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return (m_numOfLinesSeen == m_numOfLinesInSequence);
}

VfmdElementTreeNode* RefResolutionBlockLineSequence::endBlock()
{
    VfmdByteArray refId, unprocessedUrl, titleContainer;

    VfmdRegexp reFullLabelAndURL = VfmdCommonRegexps::refResolutionBlockFullLabelAndURL();
    if (reFullLabelAndURL.matches(m_linkDefText)) {
        refId = reFullLabelAndURL.capturedText(1).simplified();
        unprocessedUrl = reFullLabelAndURL.capturedText(3);
    } else {
        VfmdRegexp& reFullLabelURLAndText = VfmdCommonRegexps::refResolutionBlockFullLabelURLAndText();
        if (reFullLabelURLAndText.matches(m_linkDefText)) {
            refId = reFullLabelURLAndText.capturedText(1).simplified();
            unprocessedUrl = reFullLabelURLAndText.capturedText(3);
            titleContainer = reFullLabelURLAndText.capturedText(4);
        }
    }

    VfmdByteArray linkUrl = unprocessedUrl.bytesInStringRemoved("<> \n\t\f\r");
    VfmdByteArray linkTitle;
    if (titleContainer.isValid() && titleContainer.size() > 0) {
        VfmdRegexp reBracketedTitle = VfmdCommonRegexps::refResolutionBlockBracketedTitle();
        if (reBracketedTitle.matches(titleContainer)) {
            linkTitle = reBracketedTitle.capturedText(1);
        } else {
            char firstByte = titleContainer.byteAt(0);
            if (firstByte == '\'') {
                VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithSingleQuotedString();
                if (reQuotedString.matches(titleContainer)) {
                    linkTitle = reQuotedString.capturedText(1);
                }
            } else if (firstByte == '\"') {
                VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithDoubleQuotedString();
                if (reQuotedString.matches(titleContainer)) {
                    linkTitle = reQuotedString.capturedText(1);
                }
            }
        }
    }

    if ((refId.size() > 0) && (linkUrl.size() > 0)) {
        m_linkRefMap->add(refId.toLowerCase(), linkUrl, linkTitle);
    }

    return 0;
}
