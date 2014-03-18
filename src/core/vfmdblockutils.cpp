#include "vfmdblockutils.h"

bool isHorizontalRuleLine(const VfmdByteArray &lineContent)
{
    int index = lineContent.indexOfFirstNonSpace();
    if (index >= 0) {
        const char *data = lineContent.data();
        unsigned int sz = lineContent.size();
        const char hrByte = data[index];
        if (hrByte == '*' || hrByte == '-' || hrByte == '_') {
            int numOfHrBytes = 0;
            for (unsigned int i = index; i < sz; i++) {
                const char c = data[i];
                if (c == hrByte) {
                    numOfHrBytes++;
                } else if (c == ' ') {
                    // Nothing to do
                } else {
                    return false;
                }
            }
            if (numOfHrBytes >= 3) {
                return true;
            }
        }
    }
    return false;
}

int numOfBlockquotePrefixBytes(const VfmdByteArray &lineContent)
{
    int index = lineContent.indexOfFirstNonSpace();
    if (index >= 0 && index < 4) {
        const char bqByte = lineContent.byteAt(index);
        if (bqByte == '>') {
            if (((index + 1) < lineContent.size()) && (lineContent.byteAt(index + 1) == ' ')) {
                return (index + 2);
            }
            return (index + 1);
        }
    }
    return 0;
}
