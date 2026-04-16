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
        strcpy(t.raw, line);

        if (strncmp(line, "IF", 2) == 0 ||
            strncmp(line, "GOTO", 4) == 0 ||
            strchr(line, ':') != NULL) {
            t.is_control = 1;
        }
        else if (sscanf(line, "%s = %s %s %s", t.lhs, t.op1, t.op, t.op2) == 4) {
            t.is_binary = 1;
        }
        else if (sscanf(line, "%s = %s", t.lhs, t.op1) == 2) {
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
        if (code[i].is_control) {
            printf("%s\n", code[i].raw);
        }
        else if (code[i].is_binary) {
            printf("%s = %s %s %s\n",
                   code[i].lhs, code[i].op1,
                   code[i].op, code[i].op2);
        }
        else {
            printf("%s = %s\n", code[i].lhs, code[i].op1);
        }
    }
}

/* ---------- Constant Folding ---------- */

void constant_folding() {
    printf("\n--- Constant Folding ---\n");

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

            printf("Folding: %s = %d %s %d → %d\n",
                   code[i].lhs, a, code[i].op, b, res);

            sprintf(code[i].op1, "%d", res);
            code[i].is_binary = 0;
        }
    }
}

/* ---------- Constant Propagation ---------- */

void constant_propagation() {
    printf("\n--- Constant Propagation ---\n");

    for (int i = 0; i < n; i++) {
        if (code[i].is_control) continue;

        if (!code[i].is_binary && is_number(code[i].op1)) {
            char var[20];
            strcpy(var, code[i].lhs);

            for (int j = i + 1; j < n; j++) {

                if (code[j].is_control) continue;

                if (strcmp(code[j].op1, var) == 0) {
                    printf("Propagating: %s → %s in %s\n",
                           var, code[i].op1, code[j].lhs);
                    strcpy(code[j].op1, code[i].op1);
                }

                if (code[j].is_binary &&
                    strcmp(code[j].op2, var) == 0) {
                    printf("Propagating: %s → %s in %s\n",
                           var, code[i].op1, code[j].lhs);
                    strcpy(code[j].op2, code[i].op1);
                }
            }
        }
    }
}

/* ---------- Algebraic Simplification ---------- */

void algebraic_simplification() {
    printf("\n--- Algebraic Simplification ---\n");

    for (int i = 0; i < n; i++) {

        if (code[i].is_control || !code[i].is_binary) continue;

        // x + 0 = x
        if (strcmp(code[i].op, "+") == 0) {
            if (strcmp(code[i].op2, "0") == 0) {
                printf("Simplify: %s = %s + 0 → %s\n",
                       code[i].lhs, code[i].op1, code[i].op1);
                code[i].is_binary = 0;
            }
            else if (strcmp(code[i].op1, "0") == 0) {
                printf("Simplify: %s = 0 + %s → %s\n",
                       code[i].lhs, code[i].op2, code[i].op2);
                strcpy(code[i].op1, code[i].op2);
                code[i].is_binary = 0;
            }
        }

        // x * 1 = x
        if (strcmp(code[i].op, "*") == 0) {
            if (strcmp(code[i].op2, "1") == 0) {
                printf("Simplify: %s = %s * 1 → %s\n",
                       code[i].lhs, code[i].op1, code[i].op1);
                code[i].is_binary = 0;
            }
            else if (strcmp(code[i].op1, "1") == 0) {
                printf("Simplify: %s = 1 * %s → %s\n",
                       code[i].lhs, code[i].op2, code[i].op2);
                strcpy(code[i].op1, code[i].op2);
                code[i].is_binary = 0;
            }
        }

        // x * 0 = 0
        if (strcmp(code[i].op, "*") == 0) {
            if (strcmp(code[i].op1, "0") == 0 ||
                strcmp(code[i].op2, "0") == 0) {
                printf("Simplify: %s = %s * %s → 0\n",
                       code[i].lhs, code[i].op1, code[i].op2);
                strcpy(code[i].op1, "0");
                code[i].is_binary = 0;
            }
        }

        // x - 0 = x
        if (strcmp(code[i].op, "-") == 0 &&
            strcmp(code[i].op2, "0") == 0) {
            printf("Simplify: %s = %s - 0 → %s\n",
                   code[i].lhs, code[i].op1, code[i].op1);
            code[i].is_binary = 0;
        }

        // x / 1 = x
        if (strcmp(code[i].op, "/") == 0 &&
            strcmp(code[i].op2, "1") == 0) {
            printf("Simplify: %s = %s / 1 → %s\n",
                   code[i].lhs, code[i].op1, code[i].op1);
            code[i].is_binary = 0;
        }
    }
}

/* ---------- Common Subexpression ---------- */

void common_subexpr() {
    printf("\n--- Common Subexpression Elimination ---\n");

    for (int i = 0; i < n; i++) {
        if (code[i].is_control || !code[i].is_binary) continue;

        for (int j = i + 1; j < n; j++) {

            if (code[j].is_control) continue;

            if (code[j].is_binary &&
                strcmp(code[i].op1, code[j].op1) == 0 &&
                strcmp(code[i].op2, code[j].op2) == 0 &&
                strcmp(code[i].op, code[j].op) == 0) {

                printf("Eliminating duplicate: %s reused for %s\n",
                       code[i].lhs, code[j].lhs);

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

    // Multi-pass optimization
    for (int i = 0; i < 2; i++) {
        constant_folding();
        constant_propagation();
    }

    print_TAC("After Folding + Propagation");

    algebraic_simplification();
    print_TAC("After Algebraic Simplification");

    common_subexpr();
    print_TAC("After CSE");

    FILE *out = fopen("optimized_tac.txt", "w");

    for (int i = 0; i < n; i++) {

        if (code[i].is_control) {
            fprintf(out, "%s\n", code[i].raw);
        }
        else if (code[i].is_binary) {
            fprintf(out, "%s = %s %s %s\n",
                    code[i].lhs, code[i].op1,
                    code[i].op, code[i].op2);
        }
        else {
            fprintf(out, "%s = %s\n",
                    code[i].lhs, code[i].op1);
        }
    }

    fclose(out);

    printf("\nOptimized TAC saved to optimized_tac.txt\n");

    return 0;
}
