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

    char command[20], value[50];
    char *op1, *op2, *res;

    printf("----- PHASE 4: INTERMEDIATE CODE GENERATION (TAC) -----\n");

    while (fscanf(in, "%s", command) != EOF) {
        if (strcmp(command, "PUSH") == 0) {
            fscanf(in, "%s", value);
            push(value);
        } 
        else if (strcmp(command, "ADD") == 0 || strcmp(command, "SUB") == 0 || 
                 strcmp(command, "MUL") == 0 || strcmp(command, "DIV") == 0) {
            
            op2 = pop(); // Second operand
            op1 = pop(); // First operand
            res = new_temp();
            
            char op_char;
            if (strcmp(command, "ADD") == 0) op_char = '+';
            else if (strcmp(command, "SUB") == 0) op_char = '-';
            else if (strcmp(command, "MUL") == 0) op_char = '*';
            else op_char = '/';

            fprintf(out, "%s = %s %c %s\n", res, op1, op_char, op2);
            printf("Generated: %s = %s %c %s\n", res, op1, op_char, op2);
            push(res);
        } 
        else if (strcmp(command, "STORE") == 0) {
            fscanf(in, "%s", value);
            op1 = pop();
            fprintf(out, "%s = %s\n", value, op1);
            printf("Generated: %s = %s\n", value, op1);
        }
    }

    fclose(in);
    fclose(out);
    printf("\nThree Address Code saved to 'tac.txt'\n");
    return 0;
}