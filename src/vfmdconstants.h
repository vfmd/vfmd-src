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
    TEXTSPAN_ELEMENT,
    EMPHASIS_ELEMENT,
    LINK_ELEMENT,
    USER_SPAN_ELEMENT = 100,
    MAX_USER_SPAN_ELEMENT = 255
};

enum VfmdOpeningSpanTagStackNodeType {
    UNDEFINED_STACK_NODE = -1,
    BASE_STACK_NODE = 0,
    ASTERISK_EMPHASIS_STACK_NODE,
    UNDERSCORE_EMPHASIS_STACK_NODE,
    USER_STACK_NODE = 100
};

enum RenderFormat {
    TREE_FORMAT
};

enum TreeRenderOptions {
    TREE_RENDER_INCLUDES_TEXT = 1
};

};

#endif // VFMDCONSTANTS_H
