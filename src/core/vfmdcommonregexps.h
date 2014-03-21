#ifndef VFMDCOMMONREGEXPS_H
#define VFMDCOMMONREGEXPS_H

#include "vfmdregexp.h"

// VfmdCommonRegexps:
// Returns commonly used regexps so that they can be
// pcre-compiled once and used multiple times.
// The regexps are created on demand

class VfmdCommonRegexps
{
public:
    // Static methods returning regexp instances

    // Quoted strings
    static VfmdRegexp& beginningWithSingleQuotedString();
    static VfmdRegexp& beginningWithDoubleQuotedString();

    // Reference-resolution block
    static VfmdRegexp& refResolutionBlockLabelAndPlainURL();
    static VfmdRegexp& refResolutionBlockLabelAndBracketedURL();
    static VfmdRegexp& refResolutionBlockTitleLine();
    static VfmdRegexp& refResolutionBlockFullLabelAndURL();
    static VfmdRegexp& refResolutionBlockFullLabelURLAndText();
    static VfmdRegexp& refResolutionBlockBracketedTitle();

    // Setext header
    static VfmdRegexp& setextHeaderUnderline();

    // Atx header
    static VfmdRegexp& atxHeaderLineWithHeaderText();
    static VfmdRegexp& atxHeaderLineWithoutHeaderText();

    // List
    static VfmdRegexp& unorderedListStarter();
    static VfmdRegexp& orderedListStarter();

    // Character references
    static VfmdRegexp& htmlCharacterReference();
};

#endif // VFMDCOMMONREGEXPS_H
