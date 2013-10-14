#include "VfmdPreprocessor.h"
#include <stdio.h>

void callback(void *context, const char *data, int length) {
    printf("LINE: [");
    for (int i = 0; i < length; i++) {
        printf("%c", data[i]);
    }
    printf("]\n");
}

int main()
{
    VfmdPreprocessor prep;
    prep.setLineCallback(callback);
    prep.addBytes((char*) "poda ", 5);
    prep.addBytes((char*) "dei ", 4);
    prep.addBytes((char*) "Amazing\n", 8);
    prep.addBytes((char*) "Maurice\n", 8);
}
