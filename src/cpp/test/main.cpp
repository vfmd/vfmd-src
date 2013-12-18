#include "VfmdPreprocessor.h"
#include <stdio.h>

#define BUFFER_SIZE 1024

void callback(void *context, const char *data, int length, bool isCompleteLine) {
    printf("LINE: [");
    for (int i = 0; i < length; i++) {
        printf("%c", data[i]);
    }
    printf("]");
    if (!isCompleteLine) {
        printf(" ...");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    VfmdPreprocessor prep;
    prep.setLineCallback(callback);

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
        prep.addBytes(buffer, bytesRead);
    }
    prep.end();

    fclose(inputFile);

    return 0;
}
