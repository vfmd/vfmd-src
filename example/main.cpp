#include <stdio.h>
#include <string.h>
#include "vfmddocument.h"
#include "vfmdelementregistry.h"
#include "vfmdelementtreenode.h"
#include "vfmdscopedpointer.h"
#include "vfmdoutputdevice.h"

#define BUFFER_SIZE 1024

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
    VfmdElementTreeNode::debugPrintSubtreeSequence(parseTree);
    VfmdElementTreeNode::freeSubtreeSequence(parseTree);

    return 0;
}
