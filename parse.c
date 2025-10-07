#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

// ---- helpers ----
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

int parse_inputs(vector list[]) {
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
        if (strcmp(tok[0], "list") == 0)  { list_vectors(list);  return 1; }
        // display vector
        vector *v = find_vector(list, tok[0]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[0]); return 1; }
        print_vec(tok[0], v);
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
            make_vector(list, tok[0], x, y, z);
            vector *v = find_vector(list, tok[0]);
            print_vec(tok[0], v);
            return 1;
        }
        // fall through if not numbers (might be op forms)
    }

    // name = A + B
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "+") == 0) {
        vector *v1 = find_vector(list, tok[2]);
        vector *v2 = find_vector(list, tok[4]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; add(v1, v2, &r);
        make_vector(list, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = A - B
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "-") == 0) {
        vector *v1 = find_vector(list, tok[2]);
        vector *v2 = find_vector(list, tok[4]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; sub(v1, v2, &r);
        make_vector(list, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = A * s   (vector * scalar)
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "*") == 0 && is_number(tok[4])) {
        vector *v = find_vector(list, tok[2]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[2]); return 1; }
        double s = strtod(tok[4], NULL);
        vector r; scale_vector(v, s, &r);
        make_vector(list, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // name = s * A   (scalar * vector)
    if (ntok == 5 && strcmp(tok[1], "=") == 0 && strcmp(tok[3], "*") == 0 && is_number(tok[2])) {
        vector *v = find_vector(list, tok[4]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[4]); return 1; }
        double s = strtod(tok[2], NULL);
        vector r; scale_vector(v, s, &r);
        make_vector(list, tok[0], r.x, r.y, r.z);
        vector *dst = find_vector(list, tok[0]);
        print_vec(tok[0], dst);
        return 1;
    }

    // ----- NON-ASSIGNMENT FORMS (print ans and store it) -----

    // A + B
    if (ntok == 3 && strcmp(tok[1], "+") == 0) {
        vector *v1 = find_vector(list, tok[0]);
        vector *v2 = find_vector(list, tok[2]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; add(v1, v2, &r);
        print_vec("ans", &r);
        make_vector(list, "ans", r.x, r.y, r.z);
        return 1;
    }

    // A - B
    if (ntok == 3 && strcmp(tok[1], "-") == 0) {
        vector *v1 = find_vector(list, tok[0]);
        vector *v2 = find_vector(list, tok[2]);
        if (!v1 || !v2) { printf("Error: undefined vector in expression.\n"); return 1; }
        vector r; sub(v1, v2, &r);
        print_vec("ans", &r);
        make_vector(list, "ans", r.x, r.y, r.z);
        return 1;
    }

    // A * s
    if (ntok == 3 && strcmp(tok[1], "*") == 0 && is_number(tok[2])) {
        vector *v = find_vector(list, tok[0]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[0]); return 1; }
        double s = strtod(tok[2], NULL);
        vector r; scale_vector(v, s, &r);
        print_vec("ans", &r);
        make_vector(list, "ans", r.x, r.y, r.z);
        return 1;
    }

    // s * A
    if (ntok == 3 && strcmp(tok[1], "*") == 0 && is_number(tok[0])) {
        vector *v = find_vector(list, tok[2]);
        if (!v) { printf("Error: undefined vector '%s'.\n", tok[2]); return 1; }
        double s = strtod(tok[0], NULL);
        vector r; scale_vector(v, s, &r);
        print_vec("ans", &r);
        make_vector(list, "ans", r.x, r.y, r.z);
        return 1;
    }

    // If nothing matched
    printf("Invalid command. Remember to put spaces between every token.\n");
    return 1;
}
