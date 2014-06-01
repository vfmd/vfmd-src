#include <stdio.h>
#include <stdlib.h>
#include "vfmddocument.h"
#include "vfmdelementregistry.h"
#include "vfmdinputlinesequence.h"
#include "vfmdelementtreenode.h"
#include "core/vfmdlinkrefmap.h"
#include "core/vfmdpreprocessor.h"

VfmdDocument::VfmdDocument()
    : m_isReadingPartialContent(false)
    , m_isEndOfContent(false)
    , m_linkRefMap(new VfmdLinkRefMap)
    , m_registry(VfmdElementRegistry::createRegistryForCoreSyntax())
    , m_preprocessor(0)
    , m_documentLineSequence(0)
    , m_parseTree(0)
{
}

VfmdDocument::~VfmdDocument()
{
    VfmdElementTreeNode::freeSubtreeSequence(m_parseTree);
    delete m_documentLineSequence;
    delete m_preprocessor;
    delete m_registry;
    delete m_linkRefMap;
}

void VfmdDocument::setContent(const char *data, int length)
{
    VfmdElementTreeNode::freeSubtreeSequence(m_parseTree);
    VfmdInputLineSequence lineSequence(m_registry);
    VfmdPreprocessor::preprocessByteArray(data, length, &lineSequence);
    m_parseTree = lineSequence.endSequence();
    m_isEndOfContent = true;
}

bool VfmdDocument::addPartialContent(const char *data, int length)
{
    if (m_isEndOfContent) {
        return false;
    }
    if (!m_isReadingPartialContent) {
        m_documentLineSequence = new VfmdInputLineSequence(m_registry);
        m_preprocessor = new VfmdPreprocessor(m_documentLineSequence);
        m_isReadingPartialContent = true;
    }
    assert(m_preprocessor);
    return m_preprocessor->addBytes(data, length);
}

void VfmdDocument::endOfContent()
{
    if (m_isReadingPartialContent) {
        assert(m_preprocessor);
        assert(m_documentLineSequence);
        m_preprocessor->end();
        m_parseTree = m_documentLineSequence->endSequence();
        m_isReadingPartialContent = false;
    }
    m_isEndOfContent = true;
}

void VfmdDocument::render(VfmdConstants::RenderFormat format, int renderOptions, VfmdOutputDevice *outputDevice) const
{
    if (m_isEndOfContent && m_parseTree) {
        m_parseTree->renderSequence(format, renderOptions, outputDevice);
    }
}

VfmdElementRegistry* VfmdDocument::syntaxRegistry() const
{
    return m_registry;
}

VfmdElementTreeNode* VfmdDocument::parseTree() const
{
    return (m_isEndOfContent? m_parseTree : 0);
}
