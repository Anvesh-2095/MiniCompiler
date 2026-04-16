#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    char lhs[20];
    char op1[20];
    char op[5];
    char op2[20];
    int is_binary;
} TAC;

TAC code[MAX];
int n = 0;

/* ---------- Read Optimized TAC ---------- */

void read_TAC() {
    FILE *fp = fopen("optimized_tac.txt", "r");
    if (!fp) {
        printf("Error opening optimized_tac.txt\n");
        exit(1);
    }

    char line[100];

    while (fgets(line, sizeof(line), fp)) {
        TAC t;

        if (sscanf(line, "%s = %s %s %s", t.lhs, t.op1, t.op, t.op2) == 4) {
            t.is_binary = 1;
        } else if (sscanf(line, "%s = %s", t.lhs, t.op1) == 2) {
            t.is_binary = 0;
        }

        code[n++] = t;
    }

    fclose(fp);
}

/* ---------- Check if number ---------- */

int is_number(char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
    }
    return 1;
}

/* ---------- Generate Assembly ---------- */

void generate_code() {

    FILE *out = fopen("assembly.txt", "w");

    printf("\n----- TARGET CODE GENERATION (ASSEMBLY) -----\n");

    for (int i = 0; i < n; i++) {

        if (code[i].is_binary) {

            // Load operands
            if (is_number(code[i].op1)) {
                printf("# Load immediate %s into x2\n", code[i].op1);
                fprintf(out, "# immediate %s\n", code[i].op1);
            } else {
                printf("LW x2, %s\n", code[i].op1);
                fprintf(out, "LW x2, %s\n", code[i].op1);
            }

            if (is_number(code[i].op2)) {
                printf("# Load immediate %s into x3\n", code[i].op2);
                fprintf(out, "# immediate %s\n", code[i].op2);
            } else {
                printf("LW x3, %s\n", code[i].op2);
                fprintf(out, "LW x3, %s\n", code[i].op2);
            }

            // Operation
            if (strcmp(code[i].op, "+") == 0) {
                printf("ADD x1, x2, x3\n");
                fprintf(out, "ADD x1, x2, x3\n");
            } 
            else if (strcmp(code[i].op, "-") == 0) {
                printf("SUB x1, x2, x3\n");
                fprintf(out, "SUB x1, x2, x3\n");
            } 
            else if (strcmp(code[i].op, "*") == 0) {
                printf("MUL x1, x2, x3\n");
                fprintf(out, "MUL x1, x2, x3\n");
            } 
            else if (strcmp(code[i].op, "/") == 0) {
                printf("DIV x1, x2, x3\n");
                fprintf(out, "DIV x1, x2, x3\n");
            }

            // Store result
            printf("SW x1, %s\n", code[i].lhs);
            fprintf(out, "SW x1, %s\n", code[i].lhs);
        }

        else {
            // Simple assignment

            if (is_number(code[i].op1)) {
                printf("# Assign constant %s to %s\n", code[i].op1, code[i].lhs);
                fprintf(out, "# %s = %s\n", code[i].lhs, code[i].op1);
            } else {
                printf("LW x1, %s\n", code[i].op1);
                printf("SW x1, %s\n", code[i].lhs);

                fprintf(out, "LW x1, %s\n", code[i].op1);
                fprintf(out, "SW x1, %s\n", code[i].lhs);
            }
        }

        printf("\n");
    }

    fclose(out);

    printf("Assembly saved to assembly.txt\n");
}

/* ---------- MAIN ---------- */

int main() {

    read_TAC();

    generate_code();

    return 0;
}
