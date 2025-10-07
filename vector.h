#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    double x, y, z;
    char name[10];
    int used;
} vector;

void add(vector* a, vector* b, vector* result);
void sub(vector* a, vector* b, vector* result);
void scale_vector(vector* a, double s, vector* result);

vector* find_vector(vector list[], const char* name);
vector* make_vector(vector list[], const char* name, double x, double y, double z);
void list_vectors(vector list[]);
void clear_vectors(vector list[]);

#endif
