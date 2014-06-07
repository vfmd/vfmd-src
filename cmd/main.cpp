#include <stdio.h>
#include <string.h>
#include "vfmddocument.h"
#include "vfmdelementregistry.h"
#include "core/vfmdlinkrefmap.h"
#include "vfmdelementtreenode.h"
#include "core/vfmdscopedpointer.h"
#include "vfmdoutputdevice.h"
#include "renderers/htmlrenderer.h"
#include "renderers/treerenderer.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
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
    VfmdDocument document;
    char buffer[BUFFER_SIZE];
    while(!feof(inputFile)) {
        int bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, inputFile);
        document.addPartialContent(buffer, bytesRead);
    }
    document.endOfContent();
    if (inputFile != stdin) {
        fclose(inputFile);
    }

    // Render the document
    if (isTreeFormatOutput) {
        TreeRenderer treeRenderer;
        treeRenderer.render(document.parseTree());
    } else {
        HtmlRenderer htmlRenderer;
        htmlRenderer.render(document.parseTree());
    }

    return 0;
}
