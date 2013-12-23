#include <stdio.h>
#include <string.h>
#include "vfmddocument.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    VfmdDocument document;

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
    document.end();

    fclose(inputFile);
    return 0;
}
