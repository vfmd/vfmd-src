#include "refresolutionblockhandler.h"
#include "vfmdcommonregexps.h"

void RefResolutionBlockHandler::createChildSequence(VfmdInputLineSequence *lineSequence)
{
    VfmdByteArray firstLine = lineSequence->currentLine();
    if (firstLine.indexOfFirstNonSpace() >= 4) {
        return;
    }
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
    if (reLabelAndPlainURL.matches(firstLine) ||
        reLabelAndBracketedURL.matches(firstLine)) {
        // This is the start of a ref-resolution block
        lineSequence->setChildSequence(new RefResolutionBlockLineSequence(lineSequence));
    }
}

RefResolutionBlockLineSequence::RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent)
    : VfmdBlockLineSequence(parent), m_numOfLinesSeen(0)
{
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
    VfmdByteArray firstLine = parent->currentLine();
    VfmdByteArray refDefinitionTrailingSequence;
    if (reLabelAndPlainURL.matches(firstLine)) {
        refDefinitionTrailingSequence = reLabelAndPlainURL.capturedText(2);
    } else if (reLabelAndBracketedURL.matches(firstLine)) {
        refDefinitionTrailingSequence = reLabelAndBracketedURL.capturedText(2);
    }
    bool firstLineHasTrailingNonSpaceChars = (refDefinitionTrailingSequence.indexOfFirstNonSpace() >= 0);
    if (!firstLineHasTrailingNonSpaceChars) {
        VfmdRegexp reTitleLine = VfmdCommonRegexps::refResolutionBlockTitleLine();
        VfmdByteArray secondLine = parent->nextLine();
        if (secondLine.isValid() &&
            reTitleLine.matches(secondLine)) {
            m_numOfLinesInSequence = 2;
            return;
        }
    }
    m_numOfLinesInSequence = 1;
}

void RefResolutionBlockLineSequence::processBlockLine(const VfmdLine &currentLine, bool isEndOfParentLineSequence)
{
    VfmdLine line = currentLine;
    if (m_numOfLinesSeen < (m_numOfLinesInSequence - 1)) {
        // If not the last line, remove the trailing newline
        if (line.lastByte() == '\n') {
            line.chopRight(1);
        }
    }
    m_linkDefText.append(line);
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
    // TODO: Add data to link-ref mapping dictionary
    printf("Ref id: \"%s\"\n", refId.c_str());
    printf("Link url: \"%s\"\n", linkUrl.c_str());
    if (linkTitle.isValid()) {
        printf("Link title: \"%s\"\n", linkTitle.c_str());
    }
    return 0;
}
