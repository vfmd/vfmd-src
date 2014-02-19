#include <stdio.h>
#include <string.h>
#include "vfmddocument.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdscopedpointer.h"
#include "vfmdoutputdevice.h"

#define BUFFER_SIZE 1024
// #define OUTPUT_TREE

int main(int argc, char *argv[])
{
    VfmdScopedPointer<VfmdElementRegistry> registry(VfmdElementRegistry::createRegistryWithDefaultElements());
    VfmdDocument document(registry.data());

    FILE *inputFile = 0;
    if (argc >= 2) {
        char *fileName = argv[1];
        inputFile = fopen(fileName , "rb");
        if (inputFile == NULL) {
            fputs("Cannot open input file", stderr);
            return -1;
        }
    } else {
        inputFile = stdin;
    }

    char buffer[BUFFER_SIZE];
    while(!feof(inputFile)) {
        int bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, inputFile);
        document.addBytes(buffer, bytesRead);
    }
    fclose(inputFile);

    VfmdElementTreeNode *parseTree = document.end();
    VfmdConsoleOutputDevice console;
#ifdef OUTPUT_TREE
    parseTree->renderSequence(VfmdConstants::TREE_FORMAT,
                              (VfmdConstants::TREE_RENDER_INCLUDES_TEXT),
                              &console);
#else
    parseTree->renderSequence(VfmdConstants::HTML_FORMAT,
                              (VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS |
                               VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS),
                              &console);
#endif
    VfmdElementTreeNode::freeSubtreeSequence(parseTree);

    return 0;
}
