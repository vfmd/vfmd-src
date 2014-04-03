#include <assert.h>
#include "automaticlinkhandler.h"
#include "vfmdregexp.h"
#include "vfmdspantagstack.h"
#include "core/htmltextrenderer.h"

AutomaticLinkHandler::AutomaticLinkHandler()
{
}

static bool isWordSeparatorCharacter(VfmdUnicodeProperties::GeneralCategory category)
{
    switch(category) {
    case VfmdUnicodeProperties::ucp_Zs:
    case VfmdUnicodeProperties::ucp_Zl:
    case VfmdUnicodeProperties::ucp_Zp:
    case VfmdUnicodeProperties::ucp_Pc:
    case VfmdUnicodeProperties::ucp_Pd:
    case VfmdUnicodeProperties::ucp_Ps:
    case VfmdUnicodeProperties::ucp_Pe:
    case VfmdUnicodeProperties::ucp_Pi:
    case VfmdUnicodeProperties::ucp_Pf:
    case VfmdUnicodeProperties::ucp_Po:
    case VfmdUnicodeProperties::ucp_Cc:
    case VfmdUnicodeProperties::ucp_Cf:
        return true;
    default:
        return false;
    }
}

int AutomaticLinkHandler::identifySpanTagStartingBetween(const VfmdByteArray &text,
                                                         int fromPos, int toPos,
                                                         VfmdSpanTagStack *stack) const
{
    assert(fromPos < toPos);
    assert(toPos < text.size());
    assert(text.byteAt(toPos) == ':');

    // Rewind from toPos to a potential auto-link starting position
    int startPos = -1;
    for (unsigned int i = toPos; i >= fromPos; ) {
        if (i == 0) {
            startPos = i;
            break;
        } else {
            assert(i > 0);
            if (text.byteAt(i) == '<') {
                startPos = i;
                break;
            }
            int iPrev = text.previousUTF8CharStartsAt(i);
            if (isWordSeparatorCharacter(text.categoryOfUTF8CharStartingAt(iPrev))) {
                startPos = i;
                break;
            }
            i = iPrev;
        }
    }

    if (startPos < 0) {
        // No auto-link starting position found at or after fromPos
        return 0;
    }

    VfmdByteArray remainingText = text.mid(startPos);
    int matchingLength = -1;
    VfmdByteArray urlText;

    if (text.byteAt(startPos) == '<') {

        // Regexp that matches "<http://example.net>"
        static VfmdRegexp reBracketedAutoUrlLink("^<([a-z0-9\\+\\.\\-]+:\\/\\/[^<> \\`]+)>");

        if (reBracketedAutoUrlLink.matches(remainingText)) {
            matchingLength = reBracketedAutoUrlLink.lengthOfMatch();
            urlText = reBracketedAutoUrlLink.capturedText(1);
        } else {

            // Regexp that matches "<mailto:someone@example.net?subject=Hi+there>"
            static VfmdRegexp reBracketedAutoMailtoLink1("<(mailto:[^<> \\`]+)>");

            if (reBracketedAutoMailtoLink1.matches(remainingText)) {
                matchingLength = reBracketedAutoMailtoLink1.lengthOfMatch();
                urlText = reBracketedAutoMailtoLink1.capturedText(1);
            } else {

                // Regexp that matches "<someone@example.net>"
                static VfmdRegexp reBracketedAutoMailtoLink2("<([^\\/\\?#@\\`\\s]+@[^\\/\\?#@\\`\\s\\.]+\\.[^\\/\\?#@\\`\\s]+)>");

                if (reBracketedAutoMailtoLink2.matches(remainingText)) {
                    matchingLength = reBracketedAutoMailtoLink2.lengthOfMatch();
                    urlText = reBracketedAutoMailtoLink2.capturedText(1);
                }

            }
        }

    } else { // else of if (text.byteAt(startPos) == '<')

        // Regexp that matches "http://example.net"
        static VfmdRegexp reUnbracketedAutoUrlLink("^([a-z0-9\\+\\.\\-]+:\\/\\/)[^<>\\`\\s]+");

        if (reUnbracketedAutoUrlLink.matches(remainingText)) {

            VfmdByteArray urlCandidate = reUnbracketedAutoUrlLink.capturedText(0);
            VfmdByteArray schemeString = reUnbracketedAutoUrlLink.capturedText(1);

            unsigned int i = urlCandidate.size();
            while (i > 0) {
                unsigned int iPrev = urlCandidate.previousUTF8CharStartsAt(i);
                if ((urlCandidate.byteAt(iPrev) == '/') ||
                    (!isWordSeparatorCharacter(urlCandidate.categoryOfUTF8CharStartingAt(iPrev)))) {
                    break;
                }
                i = iPrev;
            }
            urlCandidate.chopRight(urlCandidate.size() - i);

            if (urlCandidate.size() > schemeString.size()) {
                urlText = urlCandidate;
                matchingLength = urlText.size();
            }
        }
    }

    if (matchingLength > 0) {
        // We found an automatic link
        AutomaticLinkTreeNode *autoLinkNode = new AutomaticLinkTreeNode(urlText.simplified());
        if (fromPos < startPos) {
            stack->topNode()->appendToContainedElements(text.mid(fromPos, startPos - fromPos));
        }
        stack->topNode()->appendToContainedElements(autoLinkNode);
        return (startPos + matchingLength - fromPos);
    }

    // No automatic link found
    stack->topNode()->appendToContainedElements(text.mid(fromPos, toPos - fromPos + 1));
    return (toPos - fromPos + 1);
}

void AutomaticLinkTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                       VfmdOutputDevice *outputDevice,
                                       VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write("<a href=\"", 9);
        HtmlTextRenderer::render(m_url, outputDevice,
                                 (HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                  HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
        outputDevice->write("\">", 2);
        HtmlTextRenderer::render(m_url, outputDevice,
                                 (HtmlTextRenderer::HTML_ESCAPE_ALL_LT_GT |
                                  HtmlTextRenderer::HTML_ESCAPE_AMP_UNLESS_CHARACTER_REFERENCE |
                                  HtmlTextRenderer::HTML_ESCAPE_ALL_QUOTE));
        outputDevice->write("</a>", 4);
        assert(hasChildren() == false);
    } else {
        VfmdElementTreeNode::renderNode(format, renderOptions, outputDevice, ancestorNodes);
    }
}
