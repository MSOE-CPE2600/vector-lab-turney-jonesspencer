/**
 * @file vector.h
 * @brief Header for vector math and storage management.
 *
 * Course: CPE2600
 * Section: 121
 * Assignment: Lab 5 — Vector Calculator
 * Name: Spencer Jones
 */
#ifndef VECTOR_H
#define VECTOR_H

typedef struct vector{
    double x, y, z;
    char name[10];
    int used;
} vector;

void add(vector* a, vector* b, vector* result);
void sub(vector* a, vector* b, vector* result);
void scale_vector(vector* a, double s, vector* result);

vector* find_vector(vector list[], int count, const char* name);
vector* make_vector(vector list[], int* count, const char* name, double x, double y, double z);
void list_vectors(vector list[], int count);
void clear_vectors(vector list[]);

#endif
