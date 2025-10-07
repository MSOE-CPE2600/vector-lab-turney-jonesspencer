#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parse.h"
#include "vector.h"

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        printf("Usage Summary\n");
        printf("list to list vectors\nclear to clear all vectors\n");
        printf("Show a vectors values by only entering its name\n");
        printf("Create a vector ie a = 5, 5, 5\n");
        printf("Add a vector ie a = b + c\n");
        printf("Sub a vector ie a = b - c\n");
        printf("Scale a vector ie a = b * 5\n");
    }
    vector list[10] = {0};
    printf("MiniMat Vector Calculator (type 'quit' to exit)\n");
    while (1) {
        if (!parse_inputs(list))
            break;
    }
    printf("Goodbye.\n");
    return 0;
}
