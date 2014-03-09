#include "vfmdblockutils.h"

bool isHorizontalRuleLine(const VfmdLine &line)
{
    int index = line.indexOfFirstNonSpace();
    if (index >= 0) {
        const char *data = line.data();
        unsigned int sz = line.size();
        const char hrByte = data[index];
        if (hrByte == '*' || hrByte == '-' || hrByte == '_') {
            int numOfHrBytes = 0;
            for (unsigned int i = index; i < sz; i++) {
                const char c = data[i];
                if (c == hrByte) {
                    numOfHrBytes++;
                } else if (c == ' ' || c == '\n') {
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

int numOfBlockquotePrefixBytes(const VfmdLine &line)
{
    int index = line.indexOfFirstNonSpace();
    if (index >= 0 && index < 4) {
        const char bqByte = line.byteAt(index);
        if (bqByte == '>') {
            if (((index + 1) < line.size()) && (line.byteAt(index + 1) == ' ')) {
                return (index + 2);
            }
            return (index + 1);
        }
    }
    return 0;
}
