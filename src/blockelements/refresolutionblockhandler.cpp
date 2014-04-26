#include "refresolutionblockhandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdlinkrefmap.h"

RefResolutionBlockHandler::RefResolutionBlockHandler()
    : m_linkRefMap(new VfmdLinkRefMap)
{
}

RefResolutionBlockHandler::~RefResolutionBlockHandler()
{
    delete m_linkRefMap;
}

bool RefResolutionBlockHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    if ((currentLine->firstNonSpace() != '[') ||
        (currentLine->leadingSpacesCount() >= 4)) {
        return false;
    }

    VfmdByteArray firstLineText = currentLine->content();
    VfmdByteArray firstLineTrailingText;
    VfmdRegexp reLabelAndPlainURL = VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL();
    if (reLabelAndPlainURL.matches(firstLineText)) {
        firstLineTrailingText = reLabelAndPlainURL.capturedText(2);
    } else {
        VfmdRegexp reLabelAndBracketedURL = VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL();
        if (reLabelAndBracketedURL.matches(firstLineText)) {
            firstLineTrailingText = reLabelAndBracketedURL.capturedText(2);
        } else {
            return false;
        }
    }

    m_firstLine = currentLine;
    m_nextLine = ((firstLineTrailingText.indexOfFirstNonSpace() < 0) ? nextLine : 0);
    return true;
}

void RefResolutionBlockHandler::createLineSequence(VfmdInputLineSequence *lineSequence) const
{
    int numOfLinesInSequence = 1;
    VfmdByteArray linkDefText = m_firstLine->content();
    if (m_nextLine) {
        VfmdByteArray secondLineText = m_nextLine->content();
        VfmdRegexp reTitleLine = VfmdCommonRegexps::refResolutionBlockTitleLine();
        if (reTitleLine.matches(secondLineText)) {
            numOfLinesInSequence = 2;
            linkDefText.append(secondLineText);
        }
    }

    RefResolutionBlockLineSequence *s = new RefResolutionBlockLineSequence(lineSequence,
                                                numOfLinesInSequence, linkDefText, m_linkRefMap);
    lineSequence->setChildSequence(s);
}

const VfmdLinkRefMap *RefResolutionBlockHandler::linkReferenceMap() const
{
    return m_linkRefMap;
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

bool RefResolutionBlockLineSequence::isEndOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    UNUSED_ARG(currentLine);
    UNUSED_ARG(nextLine);
    return (m_numOfLinesSeen == m_numOfLinesInSequence);
}

void RefResolutionBlockLineSequence::endBlock()
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
}
