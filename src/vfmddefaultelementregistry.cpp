#include "vfmdelementregistry.h"
#include "vfmdconstants.h"
#include "core/vfmdlinkrefmap.h"

#include "blockelements/nullblockhandler.h"
#include "blockelements/refresolutionblockhandler.h"
#include "blockelements/setextheaderhandler.h"
#include "blockelements/codeblockhandler.h"
#include "blockelements/atxheaderhandler.h"
#include "blockelements/blockquotehandler.h"
#include "blockelements/horizontalrulehandler.h"
#include "blockelements/unorderedlisthandler.h"
#include "blockelements/orderedlisthandler.h"
#include "blockelements/paragraphhandler.h"

#include "spanelements/linkhandler.h"
#include "spanelements/emphasishandler.h"
#include "spanelements/codespanhandler.h"
#include "spanelements/imagehandler.h"
#include "spanelements/htmltaghandler.h"
#include "spanelements/automaticlinkhandler.h"

VfmdElementRegistry *VfmdElementRegistry::createRegistryWithDefaultElements(VfmdLinkRefMap *linkRefMap)
{
    VfmdElementRegistry *registry = new VfmdElementRegistry;

    // Block elements
    registry->appendBlockElement(VfmdConstants::NULL_BLOCK_ELEMENT, new NullBlockHandler);
    registry->appendBlockElement(VfmdConstants::REF_RESOLUTION_BLOCK_ELEMENT,
                                 new RefResolutionBlockHandler(linkRefMap));
    registry->appendBlockElement(VfmdConstants::SETEXT_HEADER_ELEMENT, new SetextHeaderHandler);
    registry->appendBlockElement(VfmdConstants::CODE_BLOCK_ELEMENT, new CodeBlockHandler);
    registry->appendBlockElement(VfmdConstants::ATX_HEADER_ELEMENT, new AtxHeaderHandler);
    registry->appendBlockElement(VfmdConstants::BLOCKQUOTE_ELEMENT, new BlockquoteHandler);
    registry->appendBlockElement(VfmdConstants::HORIZONTAL_RULE_ELEMENT, new HorizontalRuleHandler);
    registry->appendBlockElement(VfmdConstants::UNORDERED_LIST_ELEMENT, new UnorderedListHandler);
    registry->appendBlockElement(VfmdConstants::ORDERED_LIST_ELEMENT, new OrderedListHandler);
    registry->appendBlockElement(VfmdConstants::PARAGRAPH_ELEMENT, new ParagraphHandler);

    // Span elements
    registry->appendSpanElement(VfmdConstants::LINK_ELEMENT, new LinkHandler(linkRefMap),
                                "[]", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::EMPHASIS_ELEMENT, new EmphasisHandler,
                                "*_", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::CODE_SPAN_ELEMENT, new CodeSpanHandler,
                                "`", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::IMAGE_ELEMENT, new ImageHandler(linkRefMap),
                                "!", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::HTML_ELEMENT, new HtmlTagHandler,
                                "<", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::AUTOMATIC_LINK_BRACKETED_ELEMENT, new AutomaticLinkBracketedHandler,
                                "<", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);
    registry->appendSpanElement(VfmdConstants::AUTOMATIC_LINK_ELEMENT, new AutomaticLinkHandler,
                                ":", VfmdElementRegistry::TRIGGER_BEFORE_TRIGGER_BYTE);

    return registry;
}
