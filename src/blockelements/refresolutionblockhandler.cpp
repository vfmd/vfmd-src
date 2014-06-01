#include "refresolutionblockhandler.h"
#include "core/vfmdcommonregexps.h"
#include "core/vfmdlinkrefmap.h"

RefResolutionBlockHandler::RefResolutionBlockHandler()
    : m_linkRefMap(new VfmdLinkRefMap)
    , m_numOfLines(0)
{
}

RefResolutionBlockHandler::~RefResolutionBlockHandler()
{
    delete m_linkRefMap;
}

bool RefResolutionBlockHandler::isStartOfBlock(const VfmdLine *currentLine, const VfmdLine *nextLine)
{
    m_numOfLines = 0;

    if ((currentLine->firstNonSpace() != '[') ||
        (currentLine->leadingSpacesCount() >= 4)) {
        return false;
    }

    VfmdByteArray firstLineText = currentLine->content();
    VfmdRegexp reKeyPart = VfmdCommonRegexps::refResolutionBlockStartLineKeyPart();

    if (reKeyPart.matches(firstLineText)) {

        m_rawRefIdString = reKeyPart.capturedText(1);
        VfmdByteArray refValueSequence = reKeyPart.capturedText(reKeyPart.captureCount() - 1);
        VfmdRegexp reValuePart = VfmdCommonRegexps::refResolutionBlockStartLineValuePart();

        if (reValuePart.matches(refValueSequence)) {

            m_rawUrlString = reValuePart.capturedText(1);
            m_titleContainerString = reValuePart.capturedText(2);
            m_numOfLines = 1;
            if (m_titleContainerString.indexOfFirstNonSpace() < 0 && nextLine) {
                VfmdByteArray secondLineText = nextLine->content();
                VfmdRegexp reTitleLine = VfmdCommonRegexps::refResolutionBlockTitleLine();
                if (reTitleLine.matches(secondLineText)) {
                    m_titleContainerString = secondLineText;
                    m_numOfLines = 2;
                }
            }
            return true;

        }
    }

    return false;
}

void RefResolutionBlockHandler::createLineSequence(VfmdInputLineSequence *lineSequence) const
{
    if (m_numOfLines > 0) {
        assert(m_numOfLines <= 2);
        RefResolutionBlockLineSequence *s = new RefResolutionBlockLineSequence(lineSequence,
                                                    m_rawRefIdString, m_rawUrlString, m_titleContainerString,
                                                    m_numOfLines, m_linkRefMap);
        lineSequence->setChildSequence(s);
    }
}

const VfmdLinkRefMap *RefResolutionBlockHandler::linkReferenceMap() const
{
    return m_linkRefMap;
}

RefResolutionBlockLineSequence::RefResolutionBlockLineSequence(const VfmdInputLineSequence *parent,
                                                               const VfmdByteArray &rawRefIdString,
                                                               const VfmdByteArray &rawUrlString,
                                                               const VfmdByteArray &titleContainerString,
                                                               int numOfLines,
                                                               VfmdLinkRefMap *linkRefMap)
    : VfmdBlockLineSequence(parent)
    , m_numOfLinesSeen(0)
    , m_rawRefIdString(rawRefIdString)
    , m_rawUrlString(rawUrlString)
    , m_titleContainerString(titleContainerString)
    , m_numOfLinesInSequence(numOfLines)
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
    VfmdByteArray refId = m_rawRefIdString.simplified();
    VfmdByteArray linkUrl = m_rawUrlString.bytesInStringRemoved("<> \n\t\f\r");

    VfmdByteArray linkTitle;
    m_titleContainerString.trim();
    if (m_titleContainerString.isValid() && m_titleContainerString.size() > 0) {
        VfmdRegexp reBracketedTitle = VfmdCommonRegexps::refResolutionBlockBracketedTitle();
        if (reBracketedTitle.matches(m_titleContainerString)) {
            linkTitle = reBracketedTitle.capturedText(1);
        } else {
            char firstByte = m_titleContainerString.byteAt(0);
            if (firstByte == '\'') {
                VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithSingleQuotedString();
                if (reQuotedString.matches(m_titleContainerString)) {
                    linkTitle = reQuotedString.capturedText(1);
                }
            } else if (firstByte == '\"') {
                VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithDoubleQuotedString();
                if (reQuotedString.matches(m_titleContainerString)) {
                    linkTitle = reQuotedString.capturedText(1);
                }
            }
        }
    }

    if ((refId.size() > 0) && (linkUrl.size() > 0)) {
        m_linkRefMap->add(refId.toLowerCase(), linkUrl, linkTitle);
    }
}
