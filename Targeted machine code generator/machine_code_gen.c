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
            if (!is_number(code[i].op1)) {
                printf("LW R1, %s\n", code[i].op1);
                fprintf(out, "LW R1, %s\n", code[i].op1);
            } else {
                printf("# immediate %s\n", code[i].op1);
                fprintf(out, "# immediate %s\n", code[i].op1);
            }

            if (!is_number(code[i].op2)) {
                printf("LW R2, %s\n", code[i].op2);
                fprintf(out, "LW R2, %s\n", code[i].op2);
            } else {
                printf("# immediate %s\n", code[i].op2);
                fprintf(out, "# immediate %s\n", code[i].op2);
            }

            // Operation
            if (strcmp(code[i].op, "+") == 0) {
                printf("ADD R0, R1, R2\n");
                fprintf(out, "ADD R0, R1, R2\n");
            } 
            else if (strcmp(code[i].op, "-") == 0) {
                printf("SUB R0, R1, R2\n");
                fprintf(out, "SUB R0, R1, R2\n");
            } 
            else if (strcmp(code[i].op, "*") == 0) {
                printf("MUL R0, R1, R2\n");
                fprintf(out, "MUL R0, R1, R2\n");
            } 
            else if (strcmp(code[i].op, "/") == 0) {
                printf("DIV R0, R1, R2\n");
                fprintf(out, "DIV R0, R1, R2\n");
            }

            // Store result
            printf("SW R0, %s\n", code[i].lhs);
            fprintf(out, "SW R0, %s\n", code[i].lhs);
        }

        else {
            // Assignment

            if (!is_number(code[i].op1)) {
                printf("LW R0, %s\n", code[i].op1);
                printf("SW R0, %s\n", code[i].lhs);

                fprintf(out, "LW R0, %s\n", code[i].op1);
                fprintf(out, "SW R0, %s\n", code[i].lhs);
            } else {
                printf("# %s = %s\n", code[i].lhs, code[i].op1);
                fprintf(out, "# %s = %s\n", code[i].lhs, code[i].op1);
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
