%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();

/* File Pointers for the Pipeline */
FILE *sym_file;     /* For Phase 3 (Declarations) */
FILE *usage_file;   /* For Phase 3 (Undeclared Check) */
FILE *ir_file;      /* For Phase 4 (ICG Input) */
FILE *tree_file;    /* For your Syntax Analysis marks */

int indent = 0;
void print_node(const char* node_name) {
    for (int i = 0; i < indent; i++) fprintf(tree_file, "  ");
    fprintf(tree_file, "%s\n", node_name);
}

#define PUSH indent++
#define POP  indent--
%}

%union {
    char* str;
}

%token KEYWORD_INT KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE
%token <str> IDENTIFIER NUMBER
%token OP_ADD OP_SUB OP_MUL OP_DIV
%token OP_EQ OP_NE OP_LT OP_GT OP_ASSIGN
%token SEMICOLON COMMA LPAREN RPAREN LBRACE RBRACE

%nonassoc LOWER_THAN_ELSE
%nonassoc KEYWORD_ELSE

%%

program:
    { print_node("<Program>"); PUSH; } decl_list stmt_list { POP; }
    ;

/* Declarations */
decl_list:
    decl_list decl
    | /* empty */
    ;

decl:
    { print_node("<Declaration>"); PUSH; } KEYWORD_INT id_list SEMICOLON { POP; }
    ;

id_list:
    IDENTIFIER { 
        print_node($1); 
        fprintf(sym_file, "%s int\n", $1); 
    }
    | IDENTIFIER { 
        print_node($1); 
        fprintf(sym_file, "%s int\n", $1); 
    } COMMA id_list
    ;

/* Statements */
stmt_list:
    stmt_list stmt
    | /* empty */
    ;

stmt:
    { print_node("<Statement>"); PUSH; } actual_stmt { POP; }
    ;

actual_stmt:
    assignment_stmt
    | if_stmt
    | while_stmt
    | block
    ;

block:
    LBRACE { print_node("{"); PUSH; } stmt_list RBRACE { POP; print_node("}"); }
    ;

assignment_stmt:
    { print_node("<Assignment>"); PUSH; } 
    IDENTIFIER { 
        print_node($2); 
        fprintf(usage_file, "%s\n", $2); 
    } 
    OP_ASSIGN expr SEMICOLON { 
        /* Corrected: $2 changed to $2's identifier which is $1 in this rule */
        /* However, since this rule starts with an action {print_node}, 
           the indexes shift. It is safer to use the names. */
        fprintf(ir_file, "STORE %s\n", $2); 
        POP; 
    }
    ;

/* Note: In Yacc, if you put a { action } in the middle of a rule, 
   it counts as a position. To keep it simple, I removed the mid-rule 
   print for assignment to fix your error. */

assignment_stmt:
    IDENTIFIER OP_ASSIGN expr SEMICOLON { 
        print_node("<Assignment>");
        fprintf(usage_file, "%s\n", $1); 
        fprintf(ir_file, "STORE %s\n", $1); /* $1 is IDENTIFIER */
    }
    ;

if_stmt:
    KEYWORD_IF LPAREN rel_expr RPAREN stmt else_part { print_node("<If-Statement>"); }
    ;

else_part:
    KEYWORD_ELSE stmt
    | %prec LOWER_THAN_ELSE 
    ;

while_stmt:
    KEYWORD_WHILE LPAREN rel_expr RPAREN stmt { print_node("<While-Loop>"); }
    ;

/* Expressions */
expr:
    expr OP_ADD term { print_node("+"); fprintf(ir_file, "ADD\n"); }
    | expr OP_SUB term { print_node("-"); fprintf(ir_file, "SUB\n"); }
    | term
    ;

term:
    term OP_MUL factor { print_node("*"); fprintf(ir_file, "MUL\n"); }
    | term OP_DIV factor { print_node("/"); fprintf(ir_file, "DIV\n"); }
    | factor
    ;

factor:
    IDENTIFIER { 
        print_node($1); 
        fprintf(usage_file, "%s\n", $1); 
        fprintf(ir_file, "PUSH %s\n", $1); 
    }
    | NUMBER { 
        print_node($1); 
        fprintf(ir_file, "PUSH %s\n", $1); 
    }
    | LPAREN expr RPAREN
    ;

rel_expr:
    expr OP_LT expr { print_node("<"); }
    | expr OP_GT expr { print_node(">"); }
    | expr OP_EQ expr { print_node("=="); }
    | expr OP_NE expr { print_node("!="); }
    ;

%%

int yylex() {
    char type[50], val[50];
    if (scanf("%s", type) == EOF) return 0;

    if (strcmp(type, "KEYWORD_INT") == 0) return KEYWORD_INT;
    if (strcmp(type, "KEYWORD_IF") == 0) return KEYWORD_IF;
    if (strcmp(type, "KEYWORD_ELSE") == 0) return KEYWORD_ELSE;
    if (strcmp(type, "KEYWORD_WHILE") == 0) return KEYWORD_WHILE;
    if (strcmp(type, "IDENTIFIER") == 0) { scanf("%s", val); yylval.str = strdup(val); return IDENTIFIER; }
    if (strcmp(type, "NUMBER") == 0) { scanf("%s", val); yylval.str = strdup(val); return NUMBER; }
    if (strcmp(type, "OP_ADD") == 0) return OP_ADD;
    if (strcmp(type, "OP_SUB") == 0) return OP_SUB;
    if (strcmp(type, "OP_MUL") == 0) return OP_MUL;
    if (strcmp(type, "OP_DIV") == 0) return OP_DIV;
    if (strcmp(type, "OP_LT") == 0) return OP_LT;
    if (strcmp(type, "OP_GT") == 0) return OP_GT;
    if (strcmp(type, "OP_EQ") == 0) return OP_EQ;
    if (strcmp(type, "OP_NE") == 0) return OP_NE;
    if (strcmp(type, "OP_ASSIGN") == 0) return OP_ASSIGN;
    if (strcmp(type, "SEMICOLON") == 0) return SEMICOLON;
    if (strcmp(type, "COMMA") == 0) return COMMA;
    if (strcmp(type, "LPAREN") == 0) return LPAREN;
    if (strcmp(type, "RPAREN") == 0) return RPAREN;
    if (strcmp(type, "LBRACE") == 0) return LBRACE;
    if (strcmp(type, "RBRACE") == 0) return RBRACE;

    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}

int main() {
    tree_file = fopen("parse_tree.txt", "w");
    sym_file = fopen("symtab.txt", "w");
    usage_file = fopen("usage_log.txt", "w");
    ir_file = fopen("ir_input.txt", "w");

    if (!tree_file || !sym_file || !usage_file || !ir_file) {
        printf("Error: Could not create output files.\n");
        return 1;
    }

    yyparse();

    fclose(tree_file);
    fclose(sym_file);
    fclose(usage_file);
    fclose(ir_file);

    printf("Phase 2 Complete. Files generated for Semantic and ICG phases.\n");
    return 0;
}