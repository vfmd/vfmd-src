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

VfmdRegexp& VfmdCommonRegexps::atxHeaderLineWithHeaderText()
{
    static VfmdRegexp re("^(#+)(.*[^#])#*$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::atxHeaderLineWithoutHeaderText()
{
    static VfmdRegexp re("^(#+)$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::blockquotePrefixWithEndingSpace()
{
    static VfmdRegexp re("^ *> ");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::blockquotePrefixWithoutEndingSpace()
{
    static VfmdRegexp re("^ *>");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::horizontalRule()
{
    static VfmdRegexp re("^ *((\\* *\\* *\\* *[\\* ]*)|(\\- *\\- *\\- *[\\- ]*)|(_ *_ *_ *[_ ]*))$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::unorderedListStarter()
{
    static VfmdRegexp re("^( *[\\*\\-\\+] +)[^ ]");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::orderedListStarter()
{
    static VfmdRegexp re("^( *([0-9]+)\\. +)[^ ]");
    return re;
}
