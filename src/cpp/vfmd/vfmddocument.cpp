#include <stdio.h>
#include <stdlib.h>
#include "vfmddocument.h"
#include "vfmdpreprocessor.h"
#include "vfmdinputlinesequence.h"

void preprocessorLineCallback(void *context, const VfmdLine &line) {
    VfmdInputLineSequence *documentLineSequence = (VfmdInputLineSequence *) context;
    documentLineSequence->addLine(line);
}

VfmdDocument::VfmdDocument()
{
    m_preprocessor = new VfmdPreprocessor;
    m_documentLineSequence = new VfmdInputLineSequence;
    m_preprocessor->setLineCallback(preprocessorLineCallback);
    m_preprocessor->setLineCallbackContext(m_documentLineSequence);
}

VfmdDocument::~VfmdDocument()
{
    delete m_preprocessor;
    delete m_documentLineSequence;
}

bool VfmdDocument::addBytes(const char *data, int length)
{
    int addedBytes = m_preprocessor->addBytes(data, length);
    return (addedBytes > 0);
}

void VfmdDocument::end()
{
    m_preprocessor->end();
    m_documentLineSequence->endSequence();
}
