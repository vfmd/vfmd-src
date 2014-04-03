#include <stdio.h>
#include <string.h>
#include "vfmddocument.h"
#include "vfmdelementregistry.h"
#include "core/vfmdlinkrefmap.h"
#include "vfmdelementtreenode.h"
#include "core/vfmdscopedpointer.h"
#include "vfmdoutputdevice.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    VfmdLinkRefMap linkRefMap;
    VfmdScopedPointer<VfmdElementRegistry> registry(VfmdElementRegistry::createRegistryWithDefaultElements(&linkRefMap));
    VfmdDocument document(registry.data());

    // Parse command-line options
    char *fileName = 0;
    FILE *inputFile = 0;
    bool isTreeFormatOutput = false;

    if (argc >= 2) {
        fileName = argv[1];
        if (fileName[0] == '-') {
            if (strcmp(fileName, "-t") == 0) {
                isTreeFormatOutput = true;
            } else {
                fprintf(stderr, "Unknown option: %s (Only -t is supported)\n", fileName);
                return -1;
            }
            if (argc >= 3) {
                fileName = argv[2];
            } else {
                fileName = 0;
            }
        }
    }
    if (fileName != 0) {
        inputFile = fopen(fileName , "rb");
        if (inputFile == NULL) {
            fputs("Cannot open input file", stderr);
            return -1;
        }
    } else {
        inputFile = stdin;
    }

    // Read the input document and pass on to VfmdDocument
    char buffer[BUFFER_SIZE];
    while(!feof(inputFile)) {
        int bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, inputFile);
        document.addBytes(buffer, bytesRead);
    }
    fclose(inputFile);

    // Get the parse tree from the VfmdDocument
    VfmdElementTreeNode *parseTree = document.end();

    // Render the tree to an output device
    VfmdConsoleOutputDevice console;
    if (isTreeFormatOutput) {
        parseTree->renderSequence(VfmdConstants::TREE_FORMAT,
                                  (VfmdConstants::TREE_RENDER_INCLUDES_TEXT),
                                  &console);
    } else {
        parseTree->renderSequence(VfmdConstants::HTML_FORMAT,
                                  (VfmdConstants::HTML_INDENT_ELEMENT_CONTENTS |
                                   VfmdConstants::HTML_RENDER_VOID_TAGS_AS_SELF_CLOSING_TAGS),
                                  &console);
    }

    // Free the tree
    VfmdElementTreeNode::freeSubtreeSequence(parseTree);

    return 0;
}
