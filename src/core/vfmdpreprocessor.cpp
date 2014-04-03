#include "vfmdpreprocessor.h"
#include "core/scanline.h"
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

static inline int consumeLines(const unsigned char *data, int length, VfmdByteArray *ba, VfmdInputLineSequence *lineSeq)
{
    const unsigned char* p = data;
    const unsigned char* const end = data + length;

    while (p < end) {

        int indexOfLFByte, numOfTabsTillLFByte;
        locateLFByte(p, (int) (end - p), &indexOfLFByte, &numOfTabsTillLFByte);

        if (indexOfLFByte < 0) {
            // LF byte not found. Therefore, we cannot consume any more lines.
            return (int) (p - data);
        } else if (indexOfLFByte == 0) {
            // First byte is LF byte
            VfmdByteArray lineContent = ba->mid(ba->size(), 0);
            lineSeq->addLine(new VfmdLine(lineContent));
            ba->appendByte('\n');
            p++;
        } else {
            // indexOfLF > 0

            int lineStartPos = ba->size();
            const unsigned char* const ptrToLFByte = (p + indexOfLFByte);
            int codePointsCount = 0;
            int result;
            while (1) {
                int numOfBytesScanned;
                int numOfCodePointsScanned;
                result = scan_line(p, (int) (ptrToLFByte - p), &numOfBytesScanned, &numOfCodePointsScanned);
                if (numOfBytesScanned > 0) {
                    ba->append(reinterpret_cast<const char *>(p), numOfBytesScanned);
                    p += numOfBytesScanned;
                    codePointsCount += numOfCodePointsScanned;
                }
                if (result != SCANLINE_EOL) {
                    if (result == SCANLINE_TAB_BYTE) {
                        // Expand tabs
                        unsigned int spacesToInsert = 0;
                        while (*p == 0x09 /* Tab */) {
                            int equivalentSpaces = (4 - (codePointsCount % 4));
                            spacesToInsert += equivalentSpaces;
                            codePointsCount += equivalentSpaces;
                            p++;
                        }
                        ba->appendByteNtimes(' ', spacesToInsert);
                    } else {
                        // Bad UTF-8 byte. Assume it's in ISO-8859-1 and convert to UTF-8.
                        unsigned char c = *p++;
                        ba->appendBytes((0xc0 | (((c) >> 6) & 0x03)), (0x80 | ((c) & 0x3f)));
                        codePointsCount++;
                    }
                } else {
                    p++;
                    break;
                }
            }

            int lineLength = ba->size() - lineStartPos;
            VfmdByteArray lineContent = ba->mid(lineStartPos, lineLength);
            if (lineContent.size() > 0 && lineContent.lastByte() == '\r') { // Convert CRLF to LF
                lineContent.chopRight(1);
            }
            lineSeq->addLine(new VfmdLine(lineContent));

            ba->appendByte('\n');
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
    m_text.reserveAdditionalBytes(length);
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
                         m_unconsumedBytes.size(), &m_text, m_lineSequence);
            m_unconsumedBytes.clear();
            data += indexOfLF + 1;
            length -= indexOfLF + 1;
        }
    }

    int bytesConsumed = consumeLines(reinterpret_cast<const unsigned char *>(data),
                                     length, &m_text, m_lineSequence);
    if (bytesConsumed < length) {
        m_unconsumedBytes = VfmdByteArray(data + bytesConsumed, length - bytesConsumed);
    }
}

void VfmdPreprocessor::end()
{
    m_unconsumedBytes.appendByte('\n');
    m_unconsumedBytes.squeeze();
    if (m_unconsumedBytes.size() > 0) {
        consumeLines(reinterpret_cast<const unsigned char *>(m_unconsumedBytes.data()),
                     m_unconsumedBytes.size(), &m_text, m_lineSequence);
        m_unconsumedBytes.clear();
    }
}
