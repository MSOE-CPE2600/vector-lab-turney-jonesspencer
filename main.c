/**
 * @file main.c
 * @brief Command-line MiniMat Vector Calculator
 *
 * Course: CPE2600
 * Section: 121
 * Assignment: Lab 5 — Vector Calculator
 * Name: Spencer Jones
 *
 * Algorithm:
 *  - If invoked with "-h", print help/usage summary.
 *  - Otherwise, initialize vector list and enter interactive prompt loop.
 *  - Pass user input to parse_inputs() until "quit" is entered.
 *  - Cleanly exit after goodbye message.
 *
 *  Build: Compile using make
 *  Run using ./minimat
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parse.h"
#include "vector.h"

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        printf("Usage Summary\n");
        printf("list to list vectors\nclear to clear all vectors\n");
        printf("load <fname> to load from a file");
        printf("save <fname> to save to a file");
        printf("Show a vectors values by only entering its name\n");
        printf("Create a vector ie a = 5, 5, 5\n");
        printf("Add a vector ie a = b + c\n");
        printf("Sub a vector ie a = b - c\n");
        printf("Scale a vector ie a = b * 5\n");
    }
    init();
    printf("MiniMat Vector Calculator (type 'quit' to exit)\n");
    while (true) {
        if (!parse_inputs())
            break;
    }
    cleanup();
    printf("Goodbye.\n");
    return 0;
}
