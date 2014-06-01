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

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockStartLineKeyPart()
{
    static VfmdRegexp re("^ *\\[(([^\\\\\\[\\]\\!]|\\\\.|\\![^\\[])*((\\!\\[([^\\\\\\[\\]]|\\\\.)*\\](\\[([^\\\\\\[\\]]|\\\\.)*\\])?)?([^\\\\\\[\\]]|\\\\.)*)*)\\] *:(.*)$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockStartLineValuePart()
{
    static VfmdRegexp re("^ *([^ \\<\\>]+|\\<[^\\<\\>]*\\>)( .*)?$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::refResolutionBlockTitleLine()
{
    static VfmdRegexp re("^ +(\\\"(([^\\\"\\\\]|\\\\.)*)\\\"|\\'(([^\\'\\\\]|\\\\.)*)\\'|\\(([^\\\\\\(\\)]|\\\\.)*\\)) *$");
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
    static VfmdRegexp re("^(#+).*[^#](#*)$");
    return re;
}

VfmdRegexp& VfmdCommonRegexps::atxHeaderLineWithoutHeaderText()
{
    static VfmdRegexp re("^(#+)$");
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

VfmdRegexp& VfmdCommonRegexps::htmlCharacterReference()
{
    static VfmdRegexp re("^&([A-Za-z0-9]+|#[0-9]+|#[Xx][0-9A-Fa-f]+);");
    return re;
}
