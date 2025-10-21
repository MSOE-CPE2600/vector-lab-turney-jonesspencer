/**
 * @file vector.c
 * @brief Implements core vector math operations for MiniMat Vector Calculator.
 *
 * Course: CPE2600
 * Section: 121
 * Assignment: Lab 5 — Vector Calculator
 * Name: Spencer Jones
 */
#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "parse.h"

void add(vector* a, vector* b, vector* r) {
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
}
void sub(vector* a, vector* b, vector* r) {
    r->x = a->x - b->x;
    r->y = a->y - b->y;
    r->z = a->z - b->z;
}
void scale_vector(vector* a, double s, vector* r) {
    r->x = a->x * s;
    r->y = a->y * s;
    r->z = a->z * s;
}

vector* find_vector(vector list[], int count, const char* name) {
    for (int i = 0; i < count; i++) {
        if (list[i].used && strcmp(list[i].name, name) == 0)
            return &list[i];
    }
    return NULL;
}

vector* make_vector(vector list[], int* count, const char* name, double x, double y, double z) {
    //resize list if needed
    list_resize();
    vector* v = find_vector(list, *count, name);
    if (v) {
        v->x = x; v->y = y; v->z = z;
        return v;
    }
    // else find free slot
    for (int i = 0; i < list_maxlength(); i++) {
        if (!list[i].used) {
            strncpy(list[i].name, name, sizeof(list[i].name)-1);
            list[i].name[sizeof(list[i].name)-1] = '\0';
            list[i].x = x; list[i].y = y; list[i].z = z;
            list[i].used = 1;
            (*count)++;
            return &list[i];
        }
    }
    printf("Memory full, cannot store more vectors.\n");
    return NULL;
}

void list_vectors(vector list[], int count) {
    int any = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].used) {
            printf("%s = %.2f %.2f %.2f\n", list[i].name, list[i].x, list[i].y, list[i].z);
            any = 1;
        }
    }
    if (!any) printf("(no vectors stored)\n");
}

void clear_vectors(vector list[]) {
    list_clear();
    printf("All vectors cleared.\n");
}
