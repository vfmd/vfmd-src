#include <stdio.h>
#include <stdlib.h>
#include "vfmddocument.h"
#include "core/vfmdpreprocessor.h"
#include "vfmdinputlinesequence.h"

VfmdDocument::VfmdDocument(const VfmdElementRegistry *registry)
{
    m_documentLineSequence = new VfmdInputLineSequence(registry);
    m_preprocessor = new VfmdPreprocessor(m_documentLineSequence);
}

VfmdDocument::~VfmdDocument()
{
    delete m_preprocessor;
    delete m_documentLineSequence;
}

void VfmdDocument::addBytes(const char *data, int length)
{
    m_preprocessor->addBytes(data, length);
}

VfmdElementTreeNode* VfmdDocument::end()
{
    m_preprocessor->end();
    VfmdElementTreeNode *parseTree = m_documentLineSequence->endSequence();
    return parseTree;
}
