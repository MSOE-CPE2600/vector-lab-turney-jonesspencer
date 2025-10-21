/**
 * @file parse.c
 * @brief Implements command parsing and execution for MiniMat Vector Calculator.
 *
 * Course: CPE2600
 * Section: 121
 * Assignment: Lab 5 — Vector Calculator
 * Name: Spencer Jones
 *
 * Algorithm:
 *  - Tokenize input by spaces.
 *  - Match patterns for commands (list, clear, quit), assignments, and operations.
 *  - do the given vector operation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vector.h"
#include "parse.h"

static vector* list = NULL;
static int sizev = 10;
static int count = 0;
static FILE* file = 0;

// ---- helpers ----

//resizes list by 10 slots if full and returns new size
void init(){
    if(list == NULL){
    list = (vector*) malloc(10 * sizeof(vector));
    memset(list, 0, 10 * sizeof(vector));
    }
}
void cleanup(){
    free(list);
}
void list_clear(){
    free(list);
    list = (vector*) malloc(10 * sizeof(vector));
    memset(list, 0, 10 * sizeof(vector));
    count = 0;
    sizev = 10;
}
int list_resize(){

    if(count == sizev){
        sizev+= 10;
        list = realloc(list,sizev*sizeof(vector));
        if(list == NULL)
        {
            printf("Error in heap allocation.\n");
            return 1;

        }
        memset(&(list[sizev-10]), 0, 10*sizeof(vector));
    }
    return 0;
}
int list_currentlength(){
    return count;
}
int list_maxlength(){
    return sizev;
}
static void loadfromfile(char* name, int size){
    file = fopen(name,"r");
    char line[1024] = {};
    if(file == NULL){
        printf("An error occured while loading:\n");
        perror("fopen");
        return;
    }
    while (fgets(line, sizeof(line), file)) {
        char buff[10] = {};
        double x,y,z;
        if (sscanf(line,"%9[^,],%lf,%lf,%lf", buff, &x, &y, &z) != 4) {
            printf("Error parsing file\n");
            fclose(file);
            return;
        }
        make_vector(list,&count,buff,x,y,z);
    }
    fclose(file);
}



static void savetofile(char* name, int size){
    file = fopen(name,"w+");
    if(file == NULL){
        printf("An error occured while saving\n");
        perror("fopen");
        return;
    }
    for(int i = 0; i < count; i++){
        if(list[i].used==0){
            continue;
        }
        if( fprintf(file,"%s,%lf,%lf,%lf\n",
            list[i].name,
            list[i].x,
            list[i].y,
            list[i].z) < 0
        ){
            printf("An error occured while saving\n");
            fclose(file);
        }

    }
    printf("Save successful\n");
    fclose(file);
}
static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}
static void strip_trailing_comma(char *s) {
    size_t n = strlen(s);
    if (n && s[n-1] == ',') s[n-1] = '\0';
}
static int is_number(const char *s) {
    // Accept floats and exponents: -1.2, 3, 4.5e-2
    char *end;
    if (s == NULL || *s == '\0') return 0;
    strtod(s, &end);
    return *end == '\0';
}
static int to_double3(char *sx, char *sy, char *sz, double *x, double *y, double *z) {
    strip_trailing_comma(sx);
    strip_trailing_comma(sy);
    strip_trailing_comma(sz);
    if (!is_number(sx) || !is_number(sy) || !is_number(sz)) return 0;
    *x = strtod(sx, NULL);
    *y = strtod(sy, NULL);
    *z = strtod(sz, NULL);
    return 1;
}
static void print_vec(const char *name, const vector *v) {
    printf("%s = %.2f %.2f %.2f\n", name, v->x, v->y, v->z);
}

int parse_inputs() {
    char line[256];
    char *tok[8];
    int ntok = 0;

    printf("minimat> ");
    if (!fgets(line, sizeof(line), stdin)) return 0;
    trim_newline(line);

    // tokenize by spaces ONLY (enforce spaces between tokens)
    for (char *p = strtok(line, " "); p && ntok < 8; p = strtok(NULL, " ")) {
        tok[ntok++] = p;
    }
    if (ntok == 0) return 1; // blank line

    // Single token: commands or display
    if (ntok == 1) {
        if (strcmp(tok[0], "quit") == 0) return 0;
        if (strcmp(tok[0], "clear") == 0) { clear_vectors(list); return 1; }
        if (strcmp(tok[0], "list") == 0)  { list_vectors(list,count);  return 1; }
        // display vector
        vector *v = find_vector(list, count, tok[0]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[0]); return 1; }
        print_vec(tok[0], v);
        return 1;
    }
    if(ntok == 2){
        if(strcmp(tok[0],"load") == 0){
            loadfromfile(tok[1],strlen(tok[1]));
        }
        if(strcmp(tok[0],"save") == 0){
            savetofile(tok[1],strlen(tok[1]));
        }
        return 1;
    }

    // ----- ASSIGNMENT FORMS -----

    // name = x y z   (5 tokens)
    if (ntok == 5 && strcmp(tok[1], "=") == 0) {
        double x, y, z;
        char a[32], b[32], c[32];
        strncpy(a, tok[2], sizeof(a)-1); a[sizeof(a)-1] = '\0';
        strncpy(b, tok[3], sizeof(b)-1); b[sizeof(b)-1] = '\0';
        strncpy(c, tok[4], sizeof(c)-1); c[sizeof(c)-1] = '\0';
        if (to_double3(a, b, c, &x, &y, &z)) {
            make_vector(list, &count, tok[0], x, y, z);
            vector *v = find_vector(list, count, tok[0]);
            print_vec(tok[0], v);
            return 1;
        }
        // fall through if not numbers (might be op forms)
    }

    // name = A + B
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "+") == 0) {
        vector *v1 = find_vector(list, count, tok[2]);
        vector *v2 = find_vector(list, count, tok[4]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; add(v1, v2, &r);
        make_vector(list, &count, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, count, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = A - B
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "-") == 0) {
        vector *v1 = find_vector(list, count, tok[2]);
        vector *v2 = find_vector(list, count, tok[4]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; sub(v1, v2, &r);
        make_vector(list, &count, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, count, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = A * s   (vector * scalar)
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "*") == 0 && is_number(tok[4])) {
        vector *v = find_vector(list, count, tok[2]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[2]); return 1; }
        double s = strtod(tok[4], NULL);
        vector r; scale_vector(v, s, &r);
        make_vector(list, &count, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, count, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = s * A   (scalar * vector)
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "*") == 0 && is_number(tok[2])) {
        vector *v = find_vector(list, count, tok[4]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[4]); return 1; }
        double s = strtod(tok[2], NULL);
        vector r; scale_vector(v, s, &r);
        make_vector(list, &count, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, count, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // ----- NON-ASSIGNMENT FORMS (print ans and store it) -----

    // A + B
    if (ntok == 3 && strcmp(tok[1], "+") == 0) {
        vector *v1 = find_vector(list, count, tok[0]);
        vector *v2 = find_vector(list, count, tok[2]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; add(v1, v2, &r);
        print_vec("ans", &r);
        make_vector(list, &count, "ans", r.x, r.y, r.z);
        return 1;
    }

    // A - B
    if (ntok == 3 && strcmp(tok[1], "-") == 0) {
        vector *v1 = find_vector(list, count, tok[0]);
        vector *v2 = find_vector(list, count, tok[2]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; sub(v1, v2, &r);
        print_vec("ans", &r);
        make_vector(list, &count, "ans", r.x, r.y, r.z);
        return 1;
    }

    // A * s
    if (ntok == 3 && strcmp(tok[1], "*") == 0 && is_number(tok[2])) {
        vector *v = find_vector(list, count, tok[0]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[0]); return 1; }
        double s = strtod(tok[2], NULL);
        vector r; scale_vector(v, s, &r);
        print_vec("ans", &r);
        make_vector(list, &count, "ans", r.x, r.y, r.z);
        return 1;
    }

    // s * A
    if (ntok == 3 && strcmp(tok[1], "*") == 0 && is_number(tok[0])) {
        vector *v = find_vector(list, count, tok[2]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[2]); return 1; }
        double s = strtod(tok[0], NULL);
        vector r; scale_vector(v, s, &r);
        print_vec("ans", &r);
        make_vector(list, &count, "ans", r.x, r.y, r.z);
        return 1;
    }

    // If nothing matched
    printf("Invalid command. Remember to put spaces between every token.\n");
    return 1;
}
