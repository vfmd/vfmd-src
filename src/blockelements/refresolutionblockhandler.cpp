#include "refresolutionblockhandler.h"
#include "vfmdcommonregexps.h"

RefResolutionBlockHandler::RefResolutionBlockHandler(VfmdLinkRefMap *linkRefMap)
    : m_linkRefMap(linkRefMap)
{
}

void RefResolutionBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence, const VfmdLine &firstLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(nextLine);
    if (firstLine.indexOfFirstNonSpace() >= 4) {
        return;
    }
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
    if (reLabelAndPlainURL.matches(firstLine) ||
        reLabelAndBracketedURL.matches(firstLine)) {
        lineSequence->setChildSequence(new RefResolutionBlockLineSequence(lineSequence, firstLine, nextLine, m_linkRefMap));
    }
}

RefResolutionBlockLineSequence::RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                                               const VfmdLine &firstLine,
                                                               const VfmdLine &nextLine,
                                                               VfmdLinkRefMap *linkRefMap)
    : VfmdBlockLineSequence(parent)
    , m_linkRefMap(linkRefMap)
    , m_numOfLinesSeen(0)
    , m_numOfLinesInSequence(0)
{
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
    VfmdByteArray refDefinitionTrailingSequence;
    if (reLabelAndPlainURL.matches(firstLine)) {
        refDefinitionTrailingSequence = reLabelAndPlainURL.capturedText(2);
    } else if (reLabelAndBracketedURL.matches(firstLine)) {
        refDefinitionTrailingSequence = reLabelAndBracketedURL.capturedText(2);
    }
    bool firstLineHasTrailingNonSpaceChars = (refDefinitionTrailingSequence.indexOfFirstNonSpace() >= 0);
    m_numOfLinesInSequence = 1;
    if (!firstLineHasTrailingNonSpaceChars) {
        VfmdRegexp reTitleLine = VfmdCommonRegexps::refResolutionBlockTitleLine();
        if (nextLine.isValid() &&
            reTitleLine.matches(nextLine)) {
            m_numOfLinesInSequence = 2;
        }
    }
    assert(m_numOfLinesInSequence > 0);
    m_linkDefText = firstLine;
    if (m_numOfLinesInSequence == 2) {
        m_linkDefText.append(nextLine);
    }
}

void RefResolutionBlockLineSequence::processBlockLine(const VfmdLine &currentLine, const VfmdLine &nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    m_numOfLinesSeen++;
}

bool RefResolutionBlockLineSequence::isEndOfBlock(const VfmdLine &currentLine, const VfmdLine &nextLine) const
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return (m_numOfLinesSeen == m_numOfLinesInSequence);
}

VfmdElementTreeNode* RefResolutionBlockLineSequence::endBlock()
{
    VfmdRegexp reFullLabelAndURL = VfmdCommonRegexps::refResolutionBlockFullLabelAndURL();
    VfmdRegexp& reFullLabelURLAndText = VfmdCommonRegexps::refResolutionBlockFullLabelURLAndText();
    VfmdByteArray refId, unprocessedUrl, titleContainer;
    if (reFullLabelAndURL.matches(m_linkDefText)) {
        refId = reFullLabelAndURL.capturedText(1).simplified();
        unprocessedUrl = reFullLabelAndURL.capturedText(3);
    } else if (reFullLabelURLAndText.matches(m_linkDefText)) {
        refId = reFullLabelURLAndText.capturedText(1).simplified();
        unprocessedUrl = reFullLabelURLAndText.capturedText(3);
        titleContainer = reFullLabelURLAndText.capturedText(4);
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
