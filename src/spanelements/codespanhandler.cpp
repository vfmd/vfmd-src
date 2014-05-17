#include "codespanhandler.h"
#include "vfmdspantagstack.h"
#include "core/vfmdutils.h"
#include "core/htmltextrenderer.h"

CodeSpanHandler::CodeSpanHandler()
{
}

int CodeSpanHandler::identifySpanTagStartingAt(const VfmdByteArray &text,
                                               int currentPos,
                                               VfmdSpanTagStack *stack) const
{
    assert(text.byteAt(currentPos) == '`');
    assert(!text.isEscapedAtPosition(currentPos));

    int openingBacktickPos, numOfOpeningBackticks;
    bool openingBacktickFound = locateByteRepetitionSequence('`', text, currentPos, true /* ignoreEscapedBytes */,
                                                             &openingBacktickPos, &numOfOpeningBackticks);
    assert(openingBacktickFound);
    assert(openingBacktickPos == currentPos);
    assert(numOfOpeningBackticks > 0);

    int closingBacktickPos = openingBacktickPos + numOfOpeningBackticks;
    int numOfClosingBackticks = 0;
    unsigned int pos = -1;
    while (numOfClosingBackticks != numOfOpeningBackticks) {
        pos = closingBacktickPos + numOfClosingBackticks;
        bool closingBacktickSequenceFound = locateByteRepetitionSequence('`', text, pos, false /* Don't ignoreEscapedBytes */,
                                                                         &closingBacktickPos, &numOfClosingBackticks);
        if (!closingBacktickSequenceFound) {
            break;
        }
    }

    if (numOfClosingBackticks == numOfOpeningBackticks) {
        // A valid code-span
        VfmdByteArray codeSpanContent = text.mid(openingBacktickPos + numOfOpeningBackticks,
                                                 closingBacktickPos - openingBacktickPos - numOfOpeningBackticks);
        CodeSpanTreeNode *codeSpanNode = new CodeSpanTreeNode(codeSpanContent);
        stack->topNode()->appendToContainedElements(codeSpanNode);
        return (closingBacktickPos + numOfClosingBackticks - currentPos);
    }

    // Not a valid code-span.
    // Interpret the opening backticks as text.
    stack->topNode()->appendToContainedElements(text.mid(openingBacktickPos, numOfOpeningBackticks));
    return numOfOpeningBackticks;
}

void CodeSpanTreeNode::renderNode(VfmdConstants::RenderFormat format, int renderOptions,
                                  VfmdOutputDevice *outputDevice,
                                  VfmdElementTreeNodeStack *ancestorNodes) const
{
    if (format == VfmdConstants::HTML_FORMAT) {
        outputDevice->write("<code>", 6);
        HtmlTextRenderer::renderCode(outputDevice, m_content.trimmed());
        outputDevice->write("</code>", 7);
        assert(hasChildren() == false);
    } else {
        renderTreePrefix(outputDevice, ancestorNodes, "+- span (code-span)\n");
        if ((renderOptions & VfmdConstants::TREE_RENDER_INCLUDES_TEXT) ==  VfmdConstants::TREE_RENDER_INCLUDES_TEXT) {
            renderTreeText(outputDevice, ancestorNodes, m_content);
        }
    }
}
