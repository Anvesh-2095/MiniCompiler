#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Helper to decide between Load Immediate (LI) and LOAD from memory */
void load_op(char *arg, char *reg, FILE *out) {
    if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
        fprintf(out, "LI %s, %s\n", reg, arg);
    } else {
        fprintf(out, "LOAD %s, %s\n", reg, arg);
    }
}

void generate_assembly(char *line, FILE *out) {
    char res[50], arg1[50], op[20], arg2[50], label[50], cond_op[20];

    // Remove trailing newline character to prevent sscanf issues
    line[strcspn(line, "\r\n")] = 0;

    // 1. Handle Labels (e.g., L1:)
    // Ensure it's a label and not an assignment (contains ':' but no '=')
    if (strchr(line, ':') && !strchr(line, '=')) {
        fprintf(out, "%s\n", line);
        return;
    }

    // 2. Handle Conditional Jumps: if i >= n goto L2
    if (sscanf(line, "if %s %s %s goto %s", arg1, cond_op, arg2, label) == 4) {
        load_op(arg1, "R1", out);
        load_op(arg2, "R2", out);
        fprintf(out, "CMP R1, R2\n");

        if (strcmp(cond_op, "<") == 0)       fprintf(out, "JLT %s\n", label);
        else if (strcmp(cond_op, ">") == 0)  fprintf(out, "JGT %s\n", label);
        else if (strcmp(cond_op, "==") == 0) fprintf(out, "JE %s\n", label);
        else if (strcmp(cond_op, "!=") == 0) fprintf(out, "JNE %s\n", label);
        else if (strcmp(cond_op, ">=") == 0) fprintf(out, "JGE %s\n", label);
        else if (strcmp(cond_op, "<=") == 0) fprintf(out, "JLE %s\n", label);
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

        if (strcmp(op, "+") == 0)      fprintf(out, "ADD R3, R1, R2\n");
        else if (strcmp(op, "-") == 0) fprintf(out, "SUB R3, R1, R2\n");
        else if (strcmp(op, "*") == 0) fprintf(out, "MUL R3, R1, R2\n");
        else if (strcmp(op, "/") == 0) fprintf(out, "DIV R3, R1, R2\n");

        fprintf(out, "STORE R3, %s\n", res);
        return;
    }

    // 5. Handle Assignments: c = t1 or a = 5
    if (sscanf(line, "%s = %s", res, arg1) == 2) {
        load_op(arg1, "R1", out);
        fprintf(out, "STORE R1, %s\n", res);
    }
}

int main() {
    FILE *in = fopen("optimized_tac.txt", "r");
    FILE *out = fopen("target.s", "w");
    char line[256];

    if (!in) {
        printf("Error: optimized_tac.txt not found. Please run the Optimizer first.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), in)) {
        // Skip lines that are just whitespace or empty
        if (strlen(line) > 1 && line[0] != '\n' && line[0] != '\r') {
            generate_assembly(line, out);
        }
    }

    fclose(in);
    fclose(out);
    printf("Compilation successful! Assembly saved to 'target.s'\n");
    return 0;
}