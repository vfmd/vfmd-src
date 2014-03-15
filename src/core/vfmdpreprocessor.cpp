#include "vfmdpreprocessor.h"
#include "scanline.h"
#include "vfmdinputlinesequence.h"
#include <stdlib.h>
#include <stdio.h>

VfmdPreprocessor::VfmdPreprocessor(VfmdInputLineSequence *lineSequence)
    : m_lineSequence(lineSequence)
{
}

VfmdPreprocessor::~VfmdPreprocessor()
{
}

static inline bool locateLFByte(const unsigned char *ptr, int length, int *indexOfLF, int *numOfTabsTillLF)
{
    int tabCount = 0;
    for (int i = 0; i < length; i++) {
        if (*ptr == 0x09 /* Tab */) {
            tabCount += 3;
        }
        if (*ptr == 0x0a /* LF */) {
            (*indexOfLF) = i;
            (*numOfTabsTillLF) = tabCount;
            return true;
        }
        ptr++;
    }
    (*indexOfLF) = -1;
    (*numOfTabsTillLF) = tabCount;
    return false;
}

static inline int consumeLines(const unsigned char *data, int length, VfmdInputLineSequence *lineSeq)
{
    register const unsigned char* p = data;
    register const unsigned char* const end = data + length;

    while (p < end) {

        int indexOfLFByte, numOfTabsTillLFByte;
        locateLFByte(p, (int) (end - p), &indexOfLFByte, &numOfTabsTillLFByte);

        if (indexOfLFByte < 0) {
            // LF byte not found. Therefore, we cannot consume any more lines.
            return (int) (p - data);
        } else if (indexOfLFByte == 0) {
            // First byte is LF byte
            lineSeq->addLine(VfmdLine(""));
            p++;
        } else {
            // indexOfLF > 0

            VfmdLine line;
            line.reserve(indexOfLFByte + numOfTabsTillLFByte * 3);

            register const unsigned char* const ptrToLFByte = (p + indexOfLFByte);
            int codePointsCount = 0;
            int result;
            while (1) {
                int numOfBytesScanned;
                int numOfCodePointsScanned;
                result = scan_line(p, (int) (ptrToLFByte - p), &numOfBytesScanned, &numOfCodePointsScanned);
                if (numOfBytesScanned > 0) {
                    line.append(reinterpret_cast<const char *>(p), numOfBytesScanned);
                    p += numOfBytesScanned;
                    codePointsCount += numOfCodePointsScanned;
                }
                if (result != SCANLINE_EOL) {
                    if (result == SCANLINE_TAB_BYTE) {
                        // Expand tabs
                        register unsigned int spacesToInsert = 0;
                        while (*p == 0x09 /* Tab */) {
                            int equivalentSpaces = (4 - (codePointsCount % 4));
                            spacesToInsert += equivalentSpaces;
                            codePointsCount += equivalentSpaces;
                            p++;
                        }
                        line.appendByteNtimes(' ', spacesToInsert);
                    } else {
                        // Bad UTF-8 byte. Assume it's in ISO-8859-1 and convert to UTF-8.
                        unsigned char c = *p++;
                        line.appendBytes((0xc0 | (((c) >> 6) & 0x03)), (0x80 | ((c) & 0x3f)));
                        codePointsCount++;
                    }
                } else {
                    p++;
                    break;
                }
            }

            if (line.size() > 0 && line.lastByte() == '\r') { // Convert CRLF to LF
                line.chopRight(1);
            }
            lineSeq->addLine(line);
        }

    } // while (p < end)

    return (int) (p - data);
}

static inline int indexOfLFByte(const char *p, int length)
{
    for (int i = 0; i < length; i++) {
        if (*p++ == '\n') {
            return i;
        }
    }
    return -1;
}

void VfmdPreprocessor::addBytes(const char *data, int length)
{
    if (m_unconsumedBytes.size() > 0) {
        int indexOfLF = indexOfLFByte(data, length);
        if (indexOfLF < 0) {
            // LF still not found
            m_unconsumedBytes.append(data, length);
            return;
        } else {
            // LF found
            if (indexOfLF > 0) {
                m_unconsumedBytes.append(data, indexOfLF + 1);
            }
            consumeLines(reinterpret_cast<const unsigned char *>(m_unconsumedBytes.data()),
                         m_unconsumedBytes.size(), m_lineSequence);
            m_unconsumedBytes.clear();
            data += indexOfLF + 1;
            length -= indexOfLF + 1;
        }
    }

    int bytesConsumed = consumeLines(reinterpret_cast<const unsigned char *>(data),
                                     length, m_lineSequence);
    if (bytesConsumed < length) {
        m_unconsumedBytes = VfmdLine(data + bytesConsumed, length - bytesConsumed);
    }
}

void VfmdPreprocessor::end()
{
    m_unconsumedBytes.squeeze();
    if (m_unconsumedBytes.size() > 0) {
        m_lineSequence->addLine(m_unconsumedBytes);
        m_unconsumedBytes.clear();
    }
}
