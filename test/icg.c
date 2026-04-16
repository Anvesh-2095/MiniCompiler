#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

char stack[MAX][50];
int top = -1;
int temp_count = 0;

void push(char *val) {
    strcpy(stack[++top], val);
}

char* pop() {
    return stack[top--];
}

char* new_temp() {
    char *t = malloc(10);
    sprintf(t, "t%d", temp_count++);
    return t;
}

int main() {
    FILE *in = fopen("ir_input.txt", "r");
    FILE *out = fopen("tac.txt", "w");
    
    if (!in || !out) {
        printf("Error: Missing ir_input.txt. Run Parser first.\n");
        return 1;
    }

    char command[20], value[50], label[50];
    char *op1, *op2, *res;

    printf("----- PHASE 4: INTERMEDIATE CODE GENERATION (TAC) -----\n");

    while (fscanf(in, "%s", command) != EOF) {
        // 1. Handling Operands
        if (strcmp(command, "PUSH") == 0) {
            fscanf(in, "%s", value);
            push(value);
        } 
        
        // 2. Handling Arithmetic and Relational Operators
        else if (strcmp(command, "ADD") == 0 || strcmp(command, "SUB") == 0 || 
                 strcmp(command, "MUL") == 0 || strcmp(command, "DIV") == 0 ||
                 strcmp(command, "LT") == 0 || strcmp(command, "GT") == 0 ||
                 strcmp(command, "EQ") == 0 || strcmp(command, "NE") == 0) {
            
            op2 = pop(); 
            op1 = pop(); 
            res = new_temp();
            
            char *op_str;
            if (strcmp(command, "ADD") == 0) op_str = "+";
            else if (strcmp(command, "SUB") == 0) op_str = "-";
            else if (strcmp(command, "MUL") == 0) op_str = "*";
            else if (strcmp(command, "DIV") == 0) op_str = "/";
            else if (strcmp(command, "LT") == 0) op_str = "<";
            else if (strcmp(command, "GT") == 0) op_str = ">";
            else if (strcmp(command, "EQ") == 0) op_str = "==";
            else op_str = "!=";

            fprintf(out, "%s = %s %s %s\n", res, op1, op_str, op2);
            printf("Generated: %s = %s %s %s\n", res, op1, op_str, op2);
            push(res);
        } 

        // 3. Handling Assignments
        else if (strcmp(command, "STORE") == 0) {
            fscanf(in, "%s", value);
            op1 = pop();
            fprintf(out, "%s = %s\n", value, op1);
            printf("Generated: %s = %s\n", value, op1);
        }

        // 4. Handling Control Flow (THE MISSING PART)
        else if (strcmp(command, "LABEL") == 0) {
            fscanf(in, "%s", value);
            fprintf(out, "%s:\n", value);
            printf("Generated Label: %s\n", value);
        }
        else if (strcmp(command, "JZ") == 0) {
            // JZ <condition> <label>
            // Note: In our parser, rel_expr result is on stack
            op1 = pop();
            char placeholder[50];
            fscanf(in, "%s", placeholder); // Reads the "temp" or "res" word and ignores it
            fscanf(in, "%s", label);       // NOW it reads the actual label (L1, L2, etc.)
            
            fprintf(out, "if %s == 0 goto %s\n", op1, label);
            printf("Generated: if %s == 0 goto %s\n", op1, label);
        }
        else if (strcmp(command, "JMP") == 0) {
            fscanf(in, "%s", label);
            fprintf(out, "goto %s\n", label);
            printf("Generated: goto %s\n", label);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}