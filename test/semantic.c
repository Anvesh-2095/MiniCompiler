#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 100

typedef struct {
    char name[50];
    char type[10];
} Symbol;

Symbol declared_symbols[MAX_SYMBOLS];
int symbol_count = 0;

// Load declarations from symtab.txt
void load_declarations() {
    FILE *f = fopen("symtab.txt", "r");
    if (!f) {
        printf("Semantic Error: Could not open symtab.txt\n");
        exit(1);
    }
    char name[50], type[10];
    while (fscanf(f, "%s %s", name, type) != EOF) {
        // Check for Redeclaration
        for (int i = 0; i < symbol_count; i++) {
            if (strcmp(declared_symbols[i].name, name) == 0) {
                printf("SEMANTIC ERROR: Variable '%s' redeclared.\n", name);
                exit(1);
            }
        }
        strcpy(declared_symbols[symbol_count].name, name);
        strcpy(declared_symbols[symbol_count].type, type);
        symbol_count++;
    }
    fclose(f);
}

// Check if a variable exists in the symbol table
int is_declared(char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(declared_symbols[i].name, name) == 0) return 1;
    }
    return 0;
}

int main() {
    printf("----- PHASE 3: SEMANTIC ANALYSIS -----\n");
    
    load_declarations();

    FILE *usage = fopen("usage_log.txt", "r");
    if (!usage) {
        printf("Semantic Error: Could not open usage_log.txt\n");
        return 1;
    }

    char var_name[50];
    int errors = 0;

    // The usage log contains names of variables encountered in statements/expressions
    while (fscanf(usage, "%s", var_name) != EOF) {
        // Skip constants (numbers)
        if (var_name[0] >= '0' && var_name[0] <= '9') continue;

        // Check for Undeclared Variables
        if (!is_declared(var_name)) {
            printf("SEMANTIC ERROR: Variable '%s' used but not declared.\n", var_name);
            errors++;
        }
    }

    if (errors == 0) {
        printf("Semantic Analysis Completed Successfully. No errors found.\n");
        // Create a 'verified' flag for the next stage
        FILE *ok = fopen("semantic_ok.txt", "w");
        fprintf(ok, "PASS");
        fclose(ok);
    } else {
        printf("Semantic Analysis Failed with %d errors.\n", errors);
        exit(1);
    }

    fclose(usage);
    return 0;
}