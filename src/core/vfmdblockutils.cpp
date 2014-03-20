#include "vfmdblockutils.h"

bool isHorizontalRuleLine(const VfmdLine *line)
{
    const unsigned int numOfLeadingSpaces = line->leadingSpacesCount();
    if (numOfLeadingSpaces >= 4) {
        return false;
    }

    char firstNonSpaceByte = line->firstNonSpace();
    if (firstNonSpaceByte != '*' && firstNonSpaceByte != '-' && firstNonSpaceByte != '_') {
        return false;
    }

    VfmdByteArray lineContent = line->content();
    const char *data = lineContent.data();
    const unsigned int sz = lineContent.size();
    const char hrByte = firstNonSpaceByte;
    int numOfHrBytes = 0;
    for (unsigned int i = numOfLeadingSpaces; i < (sz - numOfLeadingSpaces); i++) {
        const char c = data[i];
        if (c == hrByte) {
            numOfHrBytes++;
        } else if (c == ' ') {
            // Nothing to do
        } else {
            return false;
        }
    }
    return (numOfHrBytes >= 3);
}

int numOfBlockquotePrefixBytes(const VfmdLine *line)
{
    const unsigned int numOfLeadingSpaces = line->leadingSpacesCount();
    if (numOfLeadingSpaces >= 4) {
        return 0;
    }

    char firstNonSpaceByte = line->firstNonSpace();
    if (firstNonSpaceByte != '>') {
        return 0;
    }

    if ((numOfLeadingSpaces + 1) < line->size()) {
        if (line->content().byteAt(numOfLeadingSpaces + 1) == ' ') {
            return (numOfLeadingSpaces + 2);
        }
    }

    return (numOfLeadingSpaces + 1);
}
