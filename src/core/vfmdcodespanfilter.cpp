#include <assert.h>
#include "vfmdcodespanfilter.h"
#include "htmlstatewatcher.h"

VfmdCodeSpanFilter::VfmdCodeSpanFilter()
    : m_reCodeSpanMark(VfmdRegexp("^(`+)([^`]|$)"))
    , m_openBackticksCount(0)
{
}

void VfmdCodeSpanFilter::reset()
{
    m_openBackticksCount = 0;
}

static bool isEscapedAtEndOfString(bool startIsEscaped, const char *str, unsigned int len)
{
    if (len == 0) {
        return startIsEscaped;
    }
    assert(len > 0);
    if (str[len - 1] != '\\') {
        return false;
    }
    bool isEscaped = startIsEscaped;
    while (len--) {
        if (isEscaped) {
            isEscaped = false;
        } else if (*str == '\\') {
            isEscaped = true;
        }
        str++;
    }
    return isEscaped;
}

static bool findBackticksSpan(const VfmdByteArray &ba, unsigned int offset, int *startIndex, int *backticksCount)
{
    static VfmdRegexp reCodeSpanMark("^(`+)([^`]|$)");

    int indexOfFirstUnescapedBacktick = -1;
    unsigned int sz = ba.size();

    for (unsigned int i = offset; i < sz; i++) {
        if ((ba.byteAt(i) == '`') && (!isEscapedAtEndOfString(false, ba.data(), sz - i))) {
            indexOfFirstUnescapedBacktick = i;
            break;
        }
    }

    if (indexOfFirstUnescapedBacktick >= 0) {
        bool ok = reCodeSpanMark.matches(ba.mid(indexOfFirstUnescapedBacktick));
        assert(ok);
        if (!ok) {
            return false;
        }
        int numOfBackticks = (int) reCodeSpanMark.capturedText(1).size();
        (*startIndex) = indexOfFirstUnescapedBacktick;
        (*backticksCount) = numOfBackticks;
        return true;
    }
    return false;
}

void VfmdCodeSpanFilter::addFilteredLineToHtmlStateWatcher(const VfmdByteArray &lineContent, HtmlStateWatcher *watcher)
{
    unsigned int offset = 0;
    unsigned int sz = lineContent.size();

    while (offset < sz) {

        int backticksStartIndex, backticksCount;
        bool backtickFound = findBackticksSpan(lineContent, offset, &backticksStartIndex, &backticksCount);

        if (!backtickFound) {
            if (m_openBackticksCount == 0) {
                // No backticks found and we're not in a code span. Output the part after the offset.
                watcher->addText(lineContent.mid(offset));
            } else {
                // No backticks found and we're in a code span. Output nothing.
            }
            return;
        }

        if (backtickFound) {
            if (m_openBackticksCount == 0) {
                // We've found a possible start of a code span
                // Output the part of the line before the start of the code span
                assert(backticksStartIndex > offset);
                bool isEndOfAddedTextWithinHtmlQuotedAttribute;
                watcher->addText(lineContent.mid(offset, backticksStartIndex - offset), &isEndOfAddedTextWithinHtmlQuotedAttribute);
                if (isEndOfAddedTextWithinHtmlQuotedAttribute) {
                    // The backtick is inside a quoted HTML attribute,
                    // so it's not the start of a code span.
                    watcher->addText(lineContent.mid(backticksStartIndex, backticksCount));
                } else {
                    // The backtick is the start of a code span
                    m_openBackticksCount = backticksCount;
                }
            } else {
                // We've found backticks while inside a code span
                if (m_openBackticksCount == backticksCount) {
                    // We've found the end of the code span
                    watcher->addText("<code />");
                    m_openBackticksCount = 0;
                }
            }
            offset = backticksStartIndex + backticksCount;
        }

    }

    watcher->addText("\n");
}
