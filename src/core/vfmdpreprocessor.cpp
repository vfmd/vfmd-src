#include "vfmdpreprocessor.h"
#include "core/scanline.h"
#include "vfmdinputlinesequence.h"
#include <stdlib.h>
#include <stdio.h>

VfmdPreprocessor::VfmdPreprocessor(VfmdInputLineSequence *lineSequence)
    : m_lineSequence(lineSequence)
    , m_notEnoughBytesInLastAddBytesCall(false)
{
}

VfmdPreprocessor::~VfmdPreprocessor()
{
}

static inline int indexOf(unsigned char byte, const unsigned char *p, int length)
{
    for (int i = 0; i < length; i++) {
        if (*p++ == byte) {
            return i;
        }
    }
    return -1;
}

#ifdef VFMD_NO_UTF8_AUTOCORRECT_AND_TAB_EXPANSION
static inline int scan_line_for_tabs(const unsigned char* string, int length, int *numOfBytes, int *numOfCodePoints)
{
    int indexOfTab = indexOf(0x09 /* Tab */, string, length);
    if (indexOfTab >= 0) {
        // Tab found
        (*numOfBytes) = indexOfTab;
        (*numOfCodePoints) = indexOfTab;
        return SCANLINE_TAB_BYTE;
    }
    // Tab not found
    (*numOfBytes) = length;
    (*numOfCodePoints) = length;
    return SCANLINE_EOL;
}
#endif

static VfmdLine* createLineFromBytes(VfmdByteArray *ba, const unsigned char *src, int length)
{
    unsigned int lineStart = ba->size();
    if (length > 0) {
        const unsigned char* const end = (src + length);
        int codePointsCount = 0;
        while (1) {
            int numOfBytesScanned;
            int numOfCodePointsScanned;
#ifdef VFMD_NO_UTF8_AUTOCORRECT_AND_TAB_EXPANSION
            int result = scan_line_for_tabs(src, (int) (end - src), &numOfBytesScanned, &numOfCodePointsScanned);
#else
            int result = scan_line(src, (int) (end - src), &numOfBytesScanned, &numOfCodePointsScanned);
#endif
            if (numOfBytesScanned > 0) {
                ba->append(reinterpret_cast<const char *>(src), numOfBytesScanned);
                src += numOfBytesScanned;
                codePointsCount += numOfCodePointsScanned;
            }
            if (result != SCANLINE_EOL) {
                if (result == SCANLINE_TAB_BYTE) {
                    // Expand tabs
                    unsigned int spacesToInsert = 0;
                    while (*src == 0x09 /* Tab */) {
                        int equivalentSpaces = (4 - (codePointsCount % 4));
                        spacesToInsert += equivalentSpaces;
                        codePointsCount += equivalentSpaces;
                        src++;
                    }
                    ba->appendByteNtimes(' ', spacesToInsert);
                } else {
                    // Bad UTF-8 byte. Assume it's in ISO-8859-1 and convert to UTF-8.
                    unsigned char c = *src;
                    ba->appendBytes((0xc0 | (((c) >> 6) & 0x03)), (0x80 | ((c) & 0x3f)));
                    codePointsCount++;
                    src++;
                }
            } else {
                src++;
                break;
            }
        }
    }
    unsigned int lineEnd = ba->size();
    VfmdByteArray lineContent = ba->mid(lineStart, lineStart - lineEnd);
    if (lineContent.size() > 0 && lineContent.lastByte() == '\r') { // Convert CRLF to LF
        lineContent.chopRight(1);
    }
    return new VfmdLine(lineContent);
}

static inline int consumeLines(const unsigned char *data, int length, VfmdByteArray *ba, VfmdInputLineSequence *lineSeq)
{
    const unsigned char* p = data;
    const unsigned char* const end = data + length;

    while (p < end) {

        int indexOfLFByte = indexOf(0x0a /* LF */, p, (int) (end - p));
        if (indexOfLFByte < 0) {
            // LF byte not found. Therefore, we cannot consume any more lines.
            return (int) (p - data);
        } else {
            // LF byte found. Take the bytes till the LF and create a line out of that.
            lineSeq->addLine(createLineFromBytes(ba, p, indexOfLFByte));
            ba->appendByte('\n');
            p += (indexOfLFByte + 1);
        }

    } // while (p < end)

    return (int) (p - data);
}

bool VfmdPreprocessor::addBytes(const char *data, int length)
{
    if (m_notEnoughBytesInLastAddBytesCall) {
        printf("VfmdPreprocessor: Ignoring %d bytes\n", length);
        return false;
    } else {
        m_notEnoughBytesInLastAddBytesCall = (length < 8);
    }
    if (m_text.isInvalid()) {
        if (length < 3) {
            return true;
        }
        // Ignore UTF-8 Byte-Order-Mark
        if ((data[0] == '\xef') && (data[1] == '\xbb') && (data[2] == '\xbf')) {
            data += 3;
        }
    }

    m_text.reserveAdditionalBytes(length);
    if (m_unconsumedBytes.size() > 0) {
        int indexOfLF = indexOf(0x0a /* LF */, reinterpret_cast<const unsigned char *>(data), length);
        if (indexOfLF < 0) {
            // LF still not found
            m_unconsumedBytes.append(data, length);
            return true;
        } else {
            // LF found
            if (indexOfLF > 0) {
                m_unconsumedBytes.append(data, indexOfLF);
            }
            VfmdLine *line = createLineFromBytes(&m_text,
                                                 reinterpret_cast<const unsigned char *>(m_unconsumedBytes.data()),
                                                 m_unconsumedBytes.size());
            m_lineSequence->addLine(line);
            m_unconsumedBytes.clear();
            data += indexOfLF + 1;
            length -= indexOfLF + 1;
        }
    }

    int bytesConsumed = consumeLines(reinterpret_cast<const unsigned char *>(data), length,
                                     &m_text, m_lineSequence);
    if (bytesConsumed < length) {
        m_unconsumedBytes = VfmdByteArray(data + bytesConsumed, length - bytesConsumed);
    }

    return true;
}

void VfmdPreprocessor::end()
{
    if (m_unconsumedBytes.size() > 0) {
        VfmdLine *line = createLineFromBytes(&m_text,
                                             reinterpret_cast<const unsigned char *>(m_unconsumedBytes.data()),
                                             m_unconsumedBytes.size());
        m_lineSequence->addLine(line);
        m_unconsumedBytes.clear();
    }
    m_unconsumedBytes.squeeze(); // free memory
}

void VfmdPreprocessor::preprocessByteArray(const char *data, unsigned int length, VfmdInputLineSequence *lineSequence)
{
    // Ignore UTF-8 Byte-Order-Mark
    if (length >= 3) {
        if ((data[0] == '\xef') && (data[1] == '\xbb') && (data[2] == '\xbf')) {
            data += 3;
        }
    }

    VfmdByteArray preprocessedText;
    int bytesConsumed = consumeLines(reinterpret_cast<const unsigned char *>(data), length,
                                     &preprocessedText, lineSequence);
    if (bytesConsumed < length) {
        VfmdLine *line = createLineFromBytes(&preprocessedText,
                                             reinterpret_cast<const unsigned char *>(data + bytesConsumed),
                                             (length - bytesConsumed));
        lineSequence->addLine(line);
    }
}
