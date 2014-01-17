#ifndef VFMDCONSTANTS_H
#define VFMDCONSTANTS_H

class VfmdConstants {
public:

enum VfmdBlockElementType {
    PARAGRAPH_ELEMENT,
    BLOCKQUOTE_ELEMENT,
    USER_BLOCK_ELEMENT = 100,
    MAX_USER_BLOCK_ELEMENT = 255
};

enum VfmdSpanElementType {
    EMPHASIS_ELEMENT,
    LINK_ELEMENT,
    USER_SPAN_ELEMENT = 100,
    MAX_USER_SPAN_ELEMENT = 255
};

};

#endif // VFMDCONSTANTS_H
