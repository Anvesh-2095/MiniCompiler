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
    int is_control;
    char raw[100];
} TAC;

TAC code[MAX];
int n = 0;

/* ---------- Utility ---------- */

int is_number(char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
    }
    return 1;
}

/* ---------- Read TAC ---------- */

void read_TAC() {
    FILE *fp = fopen("tac.txt", "r");
    if (!fp) {
        printf("Error opening tac.txt\n");
        exit(1);
    }

    char line[100];

    while (fgets(line, sizeof(line), fp)) {
        TAC t;
        memset(&t, 0, sizeof(TAC));

        line[strcspn(line, "\n")] = 0;

        // If line contains control keywords → treat as RAW
        if (strstr(line, "IF") ||
            strstr(line, "if") ||
            strstr(line, "GOTO") ||
            strstr(line, "goto") ||
            strchr(line, ':')) {

            t.is_control = 1;
            strcpy(t.raw, line);
            code[n++] = t;
            continue;
        }

        // Binary
        if (sscanf(line, "%s = %s %s %s",
                   t.lhs, t.op1, t.op, t.op2) == 4) {
            t.is_binary = 1;
        }
        // Assignment
        else if (sscanf(line, "%s = %s",
                        t.lhs, t.op1) == 2) {
            t.is_binary = 0;
        }

        code[n++] = t;
    }

    fclose(fp);
}

/* ---------- Print TAC ---------- */

void print_TAC(char *msg) {
    printf("\n%s\n", msg);

    for (int i = 0; i < n; i++) {
        if (code[i].is_control)
            printf("%s\n", code[i].raw);
        else if (code[i].is_binary)
            printf("%s = %s %s %s\n",
                   code[i].lhs, code[i].op1,
                   code[i].op, code[i].op2);
        else
            printf("%s = %s\n",
                   code[i].lhs, code[i].op1);
    }
}

/* ---------- Constant Folding ---------- */

int constant_folding() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (code[i].is_control) continue;

        if (code[i].is_binary &&
            is_number(code[i].op1) &&
            is_number(code[i].op2)) {

            int a = atoi(code[i].op1);
            int b = atoi(code[i].op2);
            int res;

            if (strcmp(code[i].op, "+") == 0) res = a + b;
            else if (strcmp(code[i].op, "-") == 0) res = a - b;
            else if (strcmp(code[i].op, "*") == 0) res = a * b;
            else if (strcmp(code[i].op, "/") == 0) res = a / b;
            else if (strcmp(code[i].op, "<") == 0) res = a < b;
            else if (strcmp(code[i].op, ">") == 0) res = a > b;
            else if (strcmp(code[i].op, "==") == 0) res = a == b;
            else if (strcmp(code[i].op, "!=") == 0) res = a != b;
            else continue;

            printf("Folding: %s = %d %s %d -> %d\n",
                   code[i].lhs, a, code[i].op, b, res);

            sprintf(code[i].op1, "%d", res);
            code[i].is_binary = 0;
            changed = 1;
        }
    }

    return changed;
}

/* ---------- Constant Propagation ---------- */

int constant_propagation() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (code[i].is_control) continue;

        if (!code[i].is_binary && is_number(code[i].op1)) {

            char var[20];
            strcpy(var, code[i].lhs);

            for (int j = i + 1; j < n; j++) {

                if (code[j].is_control) continue;

                if (strcmp(code[j].op1, var) == 0) {
                    strcpy(code[j].op1, code[i].op1);
                    changed = 1;
                }

                if (code[j].is_binary &&
                    strcmp(code[j].op2, var) == 0) {
                    strcpy(code[j].op2, code[i].op1);
                    changed = 1;
                }
            }
        }
    }

    return changed;
}

/* ---------- Algebraic Simplification ---------- */

int algebraic_simplification() {
    int changed = 0;

    for (int i = 0; i < n; i++) {

        if (code[i].is_control || !code[i].is_binary) continue;

        if (strcmp(code[i].op, "+") == 0) {
            if (strcmp(code[i].op2, "0") == 0) {
                code[i].is_binary = 0;
                changed = 1;
            } else if (strcmp(code[i].op1, "0") == 0) {
                strcpy(code[i].op1, code[i].op2);
                code[i].is_binary = 0;
                changed = 1;
            }
        }

        if (strcmp(code[i].op, "*") == 0) {
            if (strcmp(code[i].op2, "1") == 0) {
                code[i].is_binary = 0;
                changed = 1;
            } else if (strcmp(code[i].op1, "1") == 0) {
                strcpy(code[i].op1, code[i].op2);
                code[i].is_binary = 0;
                changed = 1;
            } else if (strcmp(code[i].op1, "0") == 0 ||
                       strcmp(code[i].op2, "0") == 0) {
                strcpy(code[i].op1, "0");
                code[i].is_binary = 0;
                changed = 1;
            }
        }

        if (strcmp(code[i].op, "-") == 0 &&
            strcmp(code[i].op2, "0") == 0) {
            code[i].is_binary = 0;
            changed = 1;
        }

        if (strcmp(code[i].op, "/") == 0 &&
            strcmp(code[i].op2, "1") == 0) {
            code[i].is_binary = 0;
            changed = 1;
        }
    }

    return changed;
}

/* ---------- CSE ---------- */

void common_subexpr() {

    for (int i = 0; i < n; i++) {
        if (code[i].is_control || !code[i].is_binary) continue;

        for (int j = i + 1; j < n; j++) {

            if (code[j].is_control) continue;

            if (code[j].is_binary &&
                strcmp(code[i].op1, code[j].op1) == 0 &&
                strcmp(code[i].op2, code[j].op2) == 0 &&
                strcmp(code[i].op, code[j].op) == 0) {

                strcpy(code[j].op1, code[i].lhs);
                code[j].is_binary = 0;
            }
        }
    }
}

/* ---------- MAIN ---------- */

int main() {

    read_TAC();
    print_TAC("----- ORIGINAL TAC -----");

    int changed;

    do {
        changed = 0;
        changed |= constant_folding();
        changed |= constant_propagation();
        changed |= algebraic_simplification();
    } while (changed);

    print_TAC("After Optimization");

    common_subexpr();
    print_TAC("After CSE");

    FILE *out = fopen("optimized_tac.txt", "w");

    for (int i = 0; i < n; i++) {
        if (code[i].is_control)
            fprintf(out, "%s\n", code[i].raw);
        else if (code[i].is_binary)
            fprintf(out, "%s = %s %s %s\n",
                    code[i].lhs, code[i].op1, code[i].op, code[i].op2);
        else
            fprintf(out, "%s = %s\n",
                    code[i].lhs, code[i].op1);
    }

    fclose(out);

    printf("\nOptimized TAC saved to optimized_tac.txt\n");

    return 0;
}