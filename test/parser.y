%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();

FILE *sym_file, *usage_file, *ir_file, *tree_file;

int indent = 0;
void print_node(const char* node_name) {
    for (int i = 0; i < indent; i++) fprintf(tree_file, "  ");
    fprintf(tree_file, "%s\n", node_name);
}

/* Label Generator for If/While */
int label_count = 0;
char* new_label() {
    char* l = malloc(10);
    sprintf(l, "L%d", label_count++);
    return l;
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

/* Define types for expressions so they return their names/values */
%type <str> expr term factor rel_expr

%%

program:
    { print_node("<Program>"); PUSH; } decl_list stmt_list { POP; }
    ;

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

/* Unified Assignment Rule */
assignment_stmt:
    IDENTIFIER OP_ASSIGN expr SEMICOLON { 
        print_node("<Assignment>");
        PUSH; print_node($1); POP;
        fprintf(usage_file, "%s\n", $1); 
        fprintf(ir_file, "STORE %s\n", $1); 
    }
    ;

if_stmt:
    KEYWORD_IF LPAREN rel_expr RPAREN {
        /* 1. Visual Tree Logic */
        print_node("<If-Statement>");
        print_node("<Then-Block>");
        PUSH;

        /* 2. Functional TAC Logic */
        char *l_else = new_label();
        fprintf(ir_file, "JZ %s %s\n", $3, l_else);
        $<str>$ = l_else; /* Store else label in position 5 */
    } 
    stmt {
        /* 1. Visual Tree Logic */
        POP;

        /* 2. Functional TAC Logic */
        char *l_end = new_label();
        fprintf(ir_file, "JMP %s\n", l_end);
        fprintf(ir_file, "LABEL %s\n", $<str>5); /* Print Else label */
        $<str>$ = l_end; /* Store end label in position 7 */
    } 
    else_part {
        /* 2. Functional TAC Logic */
        fprintf(ir_file, "LABEL %s\n", $<str>7); /* Print End label */
    }
    ;

else_part:
    KEYWORD_ELSE {
        print_node("<Else-Block>");
        PUSH;
    } stmt {
        POP;
    }
    | %prec LOWER_THAN_ELSE /* empty */
    ;


while_stmt:
    KEYWORD_WHILE {
        char *l_start = new_label();
        fprintf(ir_file, "LABEL %s\n", l_start);
        $<str>$ = l_start;
    } LPAREN rel_expr RPAREN {
        char *l_end = new_label();
        fprintf(ir_file, "JZ %s %s\n", $4, l_end);
        $<str>$ = l_end;
    } stmt {
        fprintf(ir_file, "JMP %s\n", $<str>2);
        fprintf(ir_file, "LABEL %s\n", $<str>6);
        print_node("<While-Loop>");
    }
    ;

/* Expressions and TAC */
expr:
    expr OP_ADD term { print_node("+"); fprintf(ir_file, "ADD\n"); $$ = "temp"; }
    | expr OP_SUB term { print_node("-"); fprintf(ir_file, "SUB\n"); $$ = "temp"; }
    | term { $$ = $1; }
    ;

term:
    term OP_MUL factor { print_node("*"); fprintf(ir_file, "MUL\n"); $$ = "temp"; }
    | term OP_DIV factor { print_node("/"); fprintf(ir_file, "DIV\n"); $$ = "temp"; }
    | factor { $$ = $1; }
    ;

factor:
    IDENTIFIER { 
        print_node($1); 
        fprintf(usage_file, "%s\n", $1); 
        fprintf(ir_file, "PUSH %s\n", $1); 
        $$ = $1;
    }
    | NUMBER { 
        print_node($1); 
        fprintf(ir_file, "PUSH %s\n", $1); 
        $$ = $1;
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

rel_expr:
    expr OP_LT expr { print_node("<"); fprintf(ir_file, "LT\n"); $$ = "temp"; }
    | expr OP_GT expr { print_node(">"); fprintf(ir_file, "GT\n"); $$ = "temp"; }
    | expr OP_EQ expr { print_node("=="); fprintf(ir_file, "EQ\n"); $$ = "temp"; }
    | expr OP_NE expr { print_node("!="); fprintf(ir_file, "NE\n"); $$ = "temp"; }
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

    printf("Phase 2 Complete. Control flow and expressions finalized.\n");
    return 0;
}