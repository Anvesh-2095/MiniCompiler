#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void load_op(char *arg, char *reg, FILE *out) {
    if (isdigit(arg[0])) fprintf(out, "LI %s, %s\n", reg, arg);
    else fprintf(out, "LOAD %s, %s\n", reg, arg);
}

void generate_assembly(char *line, FILE *out) {
    char res[20], arg1[20], op[10], arg2[20], label[20], cond_op[10];

    // 1. Handle Labels (e.g., L1:)
    if (strchr(line, ':')) {
        fprintf(out, "%s", line);
        return;
    }

    // 2. Handle Conditional Jumps: Added support for >= and <=
    if (sscanf(line, "if %s %s %s goto %s", arg1, cond_op, arg2, label) == 4) {
        load_op(arg1, "R1", out);
        load_op(arg2, "R2", out);
        fprintf(out, "CMP R1, R2\n");

        if (strcmp(cond_op, "<") == 0) fprintf(out, "JLT %s\n", label);
        else if (strcmp(cond_op, ">") == 0) fprintf(out, "JGT %s\n", label);
        else if (strcmp(cond_op, "==") == 0) fprintf(out, "JE %s\n", label);
        else if (strcmp(cond_op, "!=") == 0) fprintf(out, "JNE %s\n", label);
        else if (strcmp(cond_op, ">=") == 0) fprintf(out, "JGE %s\n", label); // New
        else if (strcmp(cond_op, "<=") == 0) fprintf(out, "JLE %s\n", label); // New
        return;
    }

    // 3. Handle Unconditional Jumps: goto L2
    if (sscanf(line, "goto %s", label) == 1) {
        fprintf(out, "JMP %s\n", label);
        return;
    }

    // 4. Handle Binary Operations: t1 = a + b
    int fields = sscanf(line, "%s = %s %s %s", res, arg1, op, arg2);
    if (fields == 4) {
        load_op(arg1, "R1", out);
        load_op(arg2, "R2", out);

        if (strcmp(op, "+") == 0) fprintf(out, "ADD R3, R1, R2\n");
        else if (strcmp(op, "-") == 0) fprintf(out, "SUB R3, R1, R2\n");
        else if (strcmp(op, "*") == 0) fprintf(out, "MUL R3, R1, R2\n");
        else if (strcmp(op, "/") == 0) fprintf(out, "DIV R3, R1, R2\n");

        fprintf(out, "STORE R3, %s\n", res);
        return;
    }

    // 5. Handle Assignments: c = t1
    if (sscanf(line, "%s = %s", res, arg1) == 2) {
        load_op(arg1, "R1", out);
        fprintf(out, "STORE R1, %s\n", res);
    }
}

int main() {
    // Reading from optimized_tac.txt as per your latest version
    FILE *in = fopen("optimized_tac.txt", "r");
    FILE *out = fopen("target.s", "w");
    char line[100];

    if (!in) {
        printf("Error: optimized_tac.txt not found.\n");
        return 1;
    }


    while (fgets(line, sizeof(line), in)) {
        if (strlen(line) > 1) { // Skip empty lines
            generate_assembly(line, out);
        }
    }

    fclose(in);
    fclose(out);
    printf("assembly saved to 'target.s'\n");
    return 0;
}