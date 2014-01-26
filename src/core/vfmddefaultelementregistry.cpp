#include "vfmdelementregistry.h"
#include "vfmdconstants.h"

#include "blockelements/paragraphhandler.h"
#include "blockelements/blockquotehandler.h"

#include "spanelements/emphasishandler.h"

VfmdElementRegistry *VfmdElementRegistry::createRegistryWithDefaultElements()
{
    VfmdElementRegistry *registry = new VfmdElementRegistry;

    // Block elements
    registry->appendBlockElement(VfmdConstants::BLOCKQUOTE_ELEMENT, new BlockquoteHandler);
    registry->appendBlockElement(VfmdConstants::PARAGRAPH_ELEMENT, new ParagraphHandler);

    // Span elements
    registry->appendSpanElement(VfmdConstants::EMPHASIS_ELEMENT, new EmphasisHandler,
                                "*", VfmdElementRegistry::TRIGGER_AT_TRIGGER_BYTE);

    return registry;
}
