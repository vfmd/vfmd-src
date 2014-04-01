#ifndef VFMDLINKANDIMAGEUTILS_H
#define VFMDLINKANDIMAGEUTILS_H

#include "vfmdbytearray.h"
#include "vfmdregexp.h"
#include "vfmdcommonregexps.h"

static VfmdByteArray enclosedStringOfQuotedString(const VfmdByteArray &quotedString)
{
    char firstByte = quotedString.byteAt(0);
    if (firstByte == '\'') {
        VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithSingleQuotedString();
        if (reQuotedString.matches(quotedString) &&
            (reQuotedString.lengthOfMatch() == quotedString.size())) {
            return reQuotedString.capturedText(1);
        }
    } else if (firstByte == '\"') {
        VfmdRegexp reQuotedString = VfmdCommonRegexps::beginningWithDoubleQuotedString();
        if (reQuotedString.matches(quotedString) &&
            (reQuotedString.lengthOfMatch() == quotedString.size())) {
            return reQuotedString.capturedText(1);
        }
    }
    return VfmdByteArray();
}

template<class T>
static T* handlePossibleRefIdCloseTag(const VfmdByteArray &remainingText, int *consumedBytesCount)
{
    // Handles link/image close tags of the form:
    // "][ref id]"

    // Regexp that matches "] [ref id]"
    static VfmdRegexp reCloseWithRefId("^\\]\\s*\\[(([^\\\\\\[\\]\\`]|\\\\.)+)\\]");

    if (reCloseWithRefId.matches(remainingText)) {
        int matchingLength = reCloseWithRefId.lengthOfMatch();
        VfmdByteArray refIdText = reCloseWithRefId.capturedText(1);
        VfmdByteArray closeTagText = reCloseWithRefId.capturedText(0);
        (*consumedBytesCount) = matchingLength;
        return (new T(T::SEPARATE_REF, refIdText, closeTagText));
    }
    return 0;
}

template<class T>
static T* handlePossibleUrlTitleCloseTag(const VfmdByteArray &remainingText, int *consumedBytesCount)
{
    // Handles link/image close tags of the form:
    // "](url)"  (or)   "](url 'title')"  (or)   "](<url>)"  (or)  "](<url> 'title')"

    VfmdByteArray rawUrlString;
    int lengthOfCloseStart = 0;

    // Regexp that matches "] (url"
    static VfmdRegexp reCloseStartWithURL("^\\]\\s*\\(\\s*([^\\(\\)<>\\`\\s]+)");

    if (reCloseStartWithURL.matches(remainingText)) {
        lengthOfCloseStart = reCloseStartWithURL.lengthOfMatch();
        rawUrlString = reCloseStartWithURL.capturedText(1);
    } else {

        // Regexp that matches "] (<url>"
        static VfmdRegexp reCloseStartWithBracketedURL("^\\]\\s*\\(\\s*<([^<>\\`]*)>");

        if (reCloseStartWithBracketedURL.matches(remainingText) == 0) {
            lengthOfCloseStart = reCloseStartWithBracketedURL.lengthOfMatch();
            rawUrlString = reCloseStartWithBracketedURL.capturedText(1);
        }
    }

    if (lengthOfCloseStart > 0) {

        VfmdByteArray residualString = remainingText.mid(lengthOfCloseStart);
        VfmdByteArray titleString;
        int lengthOfCloseEnd = 0;

        // Regexp that matches ")"
        static VfmdRegexp reCloseEndWithoutTitle("^\\s*\\)");

        if (reCloseEndWithoutTitle.matches(residualString)) {
            lengthOfCloseEnd = reCloseEndWithoutTitle.lengthOfMatch();
        } else {

            // Regexp that matches "'title')"
            static VfmdRegexp reCloseEndWitTitle("^\\s*(\"(([^\\\\\"\\`]|\\\\.)*)\"|'(([^\\\\'\\`]|\\\\.)*)')\\s*\\)");

            if (reCloseEndWitTitle.matches(residualString)) {
                lengthOfCloseEnd = reCloseEndWitTitle.lengthOfMatch();
                VfmdByteArray attributesString = reCloseEndWitTitle.capturedText(1);
                assert(attributesString.size() > 0);
                titleString = enclosedStringOfQuotedString(attributesString);
            }

        }

        if (lengthOfCloseEnd > 0) {
            VfmdByteArray url = rawUrlString.bytesInStringRemoved(" \n\r\f"); // Space, LF, CR and FF
            VfmdByteArray title = titleString.bytesInStringRemoved("\n");     // LF only
            (*consumedBytesCount) = (lengthOfCloseStart + lengthOfCloseEnd);
            return (new T(T::NO_REF, url, title));
        }
    }

    return 0;
}

#endif // VFMDLINKANDIMAGEUTILS_H
