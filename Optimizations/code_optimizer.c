#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    char lhs[20];
    char op1[20];
    char op[5];
    char op2[20];
    int is_binary; // 1 if binary op, 0 if assignment
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
        if (sscanf(line, "%s = %s %s %s", t.lhs, t.op1, t.op, t.op2) == 4) {
            t.is_binary = 1;
        } else if (sscanf(line, "%s = %s", t.lhs, t.op1) == 2) {
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
        if (code[i].is_binary)
            printf("%s = %s %s %s\n", code[i].lhs, code[i].op1, code[i].op, code[i].op2);
        else
            printf("%s = %s\n", code[i].lhs, code[i].op1);
    }
}

/* ---------- Constant Folding ---------- */

void constant_folding() {
    printf("\n--- Constant Folding ---\n");

    for (int i = 0; i < n; i++) {
        if (code[i].is_binary && is_number(code[i].op1) && is_number(code[i].op2)) {

            int a = atoi(code[i].op1);
            int b = atoi(code[i].op2);
            int res;

            if (strcmp(code[i].op, "+") == 0) res = a + b;
            else if (strcmp(code[i].op, "-") == 0) res = a - b;
            else if (strcmp(code[i].op, "*") == 0) res = a * b;
            else res = a / b;

            printf("Folding: %s = %d %s %d → %d\n",
                   code[i].lhs, a, code[i].op, b, res);

            sprintf(code[i].op1, "%d", res);
            code[i].is_binary = 0; // becomes assignment
        }
    }
}

/* ---------- Constant Propagation ---------- */

void constant_propagation() {
    printf("\n--- Constant Propagation ---\n");

    for (int i = 0; i < n; i++) {
        if (!code[i].is_binary && is_number(code[i].op1)) {
            char var[20];
            strcpy(var, code[i].lhs);

            for (int j = i + 1; j < n; j++) {

                if (strcmp(code[j].op1, var) == 0) {
                    printf("Propagating: %s → %s in %s\n",
                           var, code[i].op1, code[j].lhs);
                    strcpy(code[j].op1, code[i].op1);
                }

                if (code[j].is_binary && strcmp(code[j].op2, var) == 0) {
                    printf("Propagating: %s → %s in %s\n",
                           var, code[i].op1, code[j].lhs);
                    strcpy(code[j].op2, code[i].op1);
                }
            }
        }
    }
}

/* ---------- Common Subexpression ---------- */

void common_subexpr() {
    printf("\n--- Common Subexpression Elimination ---\n");

    for (int i = 0; i < n; i++) {
        if (!code[i].is_binary) continue;

        for (int j = i + 1; j < n; j++) {
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

/* ---------- Dead Code Elimination ---------- */

void dead_code() {
    printf("\n--- Dead Code Elimination ---\n");

    int used[MAX] = {0};

    // mark used variables
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                if (strcmp(code[j].op1, code[i].lhs) == 0 ||
                   (code[j].is_binary && strcmp(code[j].op2, code[i].lhs) == 0)) {
                    used[i] = 1;
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (!used[i] && code[i].lhs[0] == 't') {
            printf("Removing dead code: %s\n", code[i].lhs);

            // shift left
            for (int j = i; j < n - 1; j++)
                code[j] = code[j + 1];

            n--;
            i--;
        }
    }
}

/* ---------- MAIN ---------- */

int main() {

    read_TAC();

    print_TAC("----- ORIGINAL TAC -----");

    constant_folding();
    print_TAC("After Constant Folding");

    constant_propagation();
    print_TAC("After Constant Propagation");

    common_subexpr();
    print_TAC("After CSE");

    dead_code();
    print_TAC("After Dead Code Elimination");

    FILE *out = fopen("optimized_tac.txt", "w");
    for (int i = 0; i < n; i++) {
        if (code[i].is_binary)
            fprintf(out, "%s = %s %s %s\n", code[i].lhs, code[i].op1, code[i].op, code[i].op2);
        else
            fprintf(out, "%s = %s\n", code[i].lhs, code[i].op1);
    }

    fclose(out);

    printf("\nOptimized TAC saved to optimized_tac.txt\n");

    return 0;
}
