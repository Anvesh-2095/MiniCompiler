%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();
extern FILE *yyin;

/* -------- SYMBOL TABLE -------- */
#define MAX 100

struct symbol {
    char name[50];
} symtab[MAX];

int symcount = 0;

int lookup(char *name) {
    for (int i = 0; i < symcount; i++) {
        if (strcmp(symtab[i].name, name) == 0)
            return 1;
    }
    return 0;
}

void insert(char *name) {
    if (!lookup(name)) {
        strcpy(symtab[symcount++].name, name);
    } else {
        printf("\nError: Variable '%s' redeclared\n", name);
    }
}

void check(char *name) {
    if (!lookup(name)) {
        printf("\nError: Variable '%s' not declared\n", name);
    }
}

void print_symtab() {
    printf("\n----- SYMBOL TABLE -----\n");
    for (int i = 0; i < symcount; i++) {
        printf("%s : int\n", symtab[i].name);
    }
}

%}

%union {
    int num;
    char* id;
}

%token INT IF ELSE WHILE
%token ID NUM
%token PLUS MINUS MUL DIV
%token LT GT EQ NE
%token ASSIGN
%token SEMICOLON COMMA
%token LPAREN RPAREN
%token LBRACE RBRACE

%type <id> ID

/* Fix dangling else */
%nonassoc IFX
%nonassoc ELSE

%%

program:
    decl_list stmt_list
    {
        printf("\nProgram parsed successfully!\n");
        print_symtab();
    }
;

decl_list:
    decl_list decl
    | 
;

decl:
    INT id_list SEMICOLON
    {
        printf("Declaration parsed\n");
    }
;

id_list:
    ID {
        insert($1);
    }
    | ID COMMA id_list {
        insert($1);
    }
;

stmt_list:
    stmt_list stmt
    | 
;

stmt:
      assign_stmt
    | if_stmt
    | while_stmt
    | block
;

block:
    LBRACE stmt_list RBRACE
;

assign_stmt:
    ID ASSIGN expr SEMICOLON
    {
        check($1);
        printf("Assignment parsed\n");
    }
;

if_stmt:
    IF LPAREN expr RPAREN stmt %prec IFX
    {
        printf("If parsed\n");
    }
  | IF LPAREN expr RPAREN stmt ELSE stmt
    {
        printf("If-Else parsed\n");
    }
;

while_stmt:
    WHILE LPAREN expr RPAREN stmt
    {
        printf("While parsed\n");
    }
;

/* -------- FIXED EXPRESSIONS -------- */

expr:
    rel_expr
;

rel_expr:
      rel_expr LT add_expr
    | rel_expr GT add_expr
    | rel_expr EQ add_expr
    | rel_expr NE add_expr
    | add_expr
;

add_expr:
      add_expr PLUS term
    | add_expr MINUS term
    | term
;

term:
      term MUL factor
    | term DIV factor
    | factor
;

factor:
      ID { check($1); }
    | NUM
    | LPAREN expr RPAREN
;

%%

void yyerror(const char *s) {
    printf("\nError: %s\n", s);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    printf("----- LEXICAL ANALYSIS -----\n");

    yyparse();

    fclose(yyin);
    return 0;
}
