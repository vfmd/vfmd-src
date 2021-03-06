#include "vfmdelementregistry.h"
#include "vfmdconstants.h"
#include "core/vfmdlinkrefmap.h"

#include "blockelements/refresolutionblockhandler.h"
#include "blockelements/setextheaderhandler.h"
#include "blockelements/codeblockhandler.h"
#include "blockelements/atxheaderhandler.h"
#include "blockelements/blockquotehandler.h"
#include "blockelements/horizontalrulehandler.h"
#include "blockelements/unorderedlisthandler.h"
#include "blockelements/orderedlisthandler.h"

#include "spanelements/linkhandler.h"
#include "spanelements/emphasishandler.h"
#include "spanelements/codespanhandler.h"
#include "spanelements/imagehandler.h"
#include "spanelements/htmltaghandler.h"
#include "spanelements/automaticlinkhandler.h"

VfmdElementRegistry *VfmdElementRegistry::createRegistryForCoreSyntax()
{
    VfmdElementRegistry *registry = new VfmdElementRegistry;
    RefResolutionBlockHandler *refResolutionBlock = new RefResolutionBlockHandler;

    // Block elements without trigger bytes
    registry->appendBlockElement(VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT, refResolutionBlock);
    registry->appendBlockElement(VfmdConstants::SETEXT_HEADER_ELEMENT, new SetextHeaderHandler);
    registry->appendBlockElement(VfmdConstants::CODE_BLOCK_ELEMENT, new CodeBlockHandler);

    // Block elements with trigger bytes
    registry->appendBlockElement(VfmdConstants::ATX_HEADER_ELEMENT, new AtxHeaderHandler, "#");
    registry->appendBlockElement(VfmdConstants::BLOCKQUOTE_ELEMENT, new BlockquoteHandler, ">",
                                 VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH);
    registry->appendBlockElement(VfmdConstants::HORIZONTAL_RULE_ELEMENT, new HorizontalRuleHandler, "*-_",
                                 VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH);
    registry->appendBlockElement(VfmdConstants::UNORDERED_LIST_ELEMENT, new UnorderedListHandler, "*-+",
                                 VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH);
    registry->appendBlockElement(VfmdConstants::ORDERED_LIST_ELEMENT, new OrderedListHandler, "0123456789",
                                 VfmdElementRegistry::BLOCK_CAN_ABUT_PRECEDING_PARAGRAPH);

    // Span elements with trigger bytes
    registry->appendSpanElement(VfmdConstants::LINK_ELEMENT,
                                new LinkHandler(refResolutionBlock->linkReferenceMap()), "[]");
    registry->appendSpanElement(VfmdConstants::EMPHASIS_ELEMENT, new EmphasisHandler, "*_");
    registry->appendSpanElement(VfmdConstants::CODE_SPAN_ELEMENT, new CodeSpanHandler, "`");
    registry->appendSpanElement(VfmdConstants::IMAGE_ELEMENT,
                                new ImageHandler(refResolutionBlock->linkReferenceMap()), "!");
    registry->appendSpanElement(VfmdConstants::HTML_ELEMENT, new HtmlTagHandler, "<");
    registry->appendSpanElement(VfmdConstants::AUTOMATIC_LINK_BRACKETED_ELEMENT,
                                new AutomaticLinkBracketedHandler, "<");
    registry->appendSpanElement(VfmdConstants::AUTOMATIC_LINK_ELEMENT, new AutomaticLinkHandler, ":",
                                VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE);

    return registry;
}
