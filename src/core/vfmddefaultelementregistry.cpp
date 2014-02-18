#include "vfmdelementregistry.h"
#include "vfmdconstants.h"

#include "blockelements/nullblockhandler.h"
#include "blockelements/refresolutionblockhandler.h"
#include "blockelements/setextheaderhandler.h"
#include "blockelements/codeblockhandler.h"
#include "blockelements/atxheaderhandler.h"
#include "blockelements/blockquotehandler.h"
#include "blockelements/horizontalrulehandler.h"
#include "blockelements/unorderedlisthandler.h"
#include "blockelements/paragraphhandler.h"

#include "spanelements/emphasishandler.h"

VfmdElementRegistry *VfmdElementRegistry::createRegistryWithDefaultElements()
{
    VfmdElementRegistry *registry = new VfmdElementRegistry;

    // Block elements
    registry->appendBlockElement(VfmdConstants::NULL_BLOCK_ELEMENT, new NullBlockHandler);
    registry->appendBlockElement(VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT, new RefResolutionBlockHandler);
    registry->appendBlockElement(VfmdConstants::SETEXT_HEADER_ELEMENT, new SetextHeaderHandler);
    registry->appendBlockElement(VfmdConstants::CODE_BLOCK_ELEMENT, new CodeBlockHandler);
    registry->appendBlockElement(VfmdConstants::ATX_HEADER_ELEMENT, new AtxHeaderHandler);
    registry->appendBlockElement(VfmdConstants::BLOCKQUOTE_ELEMENT, new BlockquoteHandler);
    registry->appendBlockElement(VfmdConstants::HORIZONTAL_RULE_ELEMENT, new HorizontalRuleHandler);
    registry->appendBlockElement(VfmdConstants::UNORDERED_LIST_ELEMENT, new UnorderedListHandler);
    registry->appendBlockElement(VfmdConstants::PARAGRAPH_ELEMENT, new ParagraphHandler);

    // Span elements
    registry->appendSpanElement(VfmdConstants::EMPHASIS_ELEMENT, new EmphasisHandler,
                                "*", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);

    return registry;
}
