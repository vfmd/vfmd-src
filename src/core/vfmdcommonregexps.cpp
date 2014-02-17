#include "vfmdcommonregexps.h"

VfmdRegexp& VfmdCommonRegexps::beginningWithSingleQuotedString()
{
    static VfmdRegexp re("^\\'(([^\\\\\\']|\\\\.)*)\\'");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::beginningWithDoubleQuotedString()
{
    static VfmdRegexp re("^\\\"(([^\\\\\\\"]|\\\\.)*)\\\"");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockLabelAndPlainURL()
{
    static VfmdRegexp re("^ *\\[([^\\\\\\[\\]]|\\\\.)*\\] *: *[^ <>]+( .*)?$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockLabelAndBracketedURL()
{
    static VfmdRegexp re("^ *\\[([^\\\\\\[\\]]|\\\\.)*\\] *: *<[^<>]*>(.*)$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockTitleLine()
{
    static VfmdRegexp re("^ +(\\\"(([^\\\"\\\\]|\\\\.)*)\\\"|\\'(([^\\'\\\\]|\\\\.)*)\\'|\\(([^\\\\\\(\\)]|\\\\.)*\\)) *$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockFullLabelAndURL()
{
    static VfmdRegexp re("^ *\\[(([^\\\\\\[\\]]|\\\\.)*)\\] *: *([^ <>]+|<[^<>]*>) *$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockFullLabelURLAndText()
{
    static VfmdRegexp re("^ *\\[(([^\\\\\\[\\]]|\\\\.)*)\\] *: *([^ <>]+|<[^<>]*>) +([^ ].*)$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockBracketedTitle()
{
    static VfmdRegexp re("^\\((([^\\\\\\(\\)]|\\\\.)*)\\)");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::setextHeaderUnderline()
{
    static VfmdRegexp re("^(-+|=+) *$");
    return re;
}
