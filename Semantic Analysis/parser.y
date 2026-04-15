%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();

/* DECLARE THE FILES HERE */
FILE *sym_file;
FILE *usage_file;
FILE *tree_file;

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
        fprintf(sym_file, "%s int\n", $1); /* LOG FOR SEMANTIC PHASE */
    }
    | IDENTIFIER { 
        print_node($1); 
        fprintf(sym_file, "%s int\n", $1); /* LOG FOR SEMANTIC PHASE */
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
        fprintf(usage_file, "%s\n", $2); /* LOG FOR SEMANTIC PHASE */
    } 
    OP_ASSIGN expr SEMICOLON { POP; }
    ;

if_stmt:
    { print_node("<If-Statement>"); PUSH; } KEYWORD_IF LPAREN rel_expr RPAREN stmt else_part { POP; }
    ;

else_part:
    KEYWORD_ELSE { print_node("else"); } stmt
    | %prec LOWER_THAN_ELSE 
    ;

while_stmt:
    { print_node("<While-Loop>"); PUSH; } KEYWORD_WHILE LPAREN rel_expr RPAREN stmt { POP; }
    ;

/* Expressions */
expr:
    expr OP_ADD term { print_node("+"); }
    | expr OP_SUB term { print_node("-"); }
    | term
    ;

term:
    term OP_MUL factor { print_node("*"); }
    | term OP_DIV factor { print_node("/"); }
    | factor
    ;

factor:
    IDENTIFIER { 
        print_node($1); 
        fprintf(usage_file, "%s\n", $1); /* LOG FOR SEMANTIC PHASE */
    }
    | NUMBER { print_node($1); }
    | LPAREN expr RPAREN
    ;

rel_expr:
    expr OP_LT expr { print_node("<"); }
    | expr OP_GT expr { print_node(">"); }
    | expr OP_EQ expr { print_node("=="); }
    | expr OP_NE expr { print_node("!="); }
    ;

%%

/* Pipeline Reader for tokens.txt */
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
    /* INITIALIZE ALL FILES HERE */
    tree_file = fopen("parse_tree.txt", "w");
    sym_file = fopen("symtab.txt", "w");
    usage_file = fopen("usage_log.txt", "w");

    if (!tree_file || !sym_file || !usage_file) {
        printf("Error: Could not create output files.\n");
        return 1;
    }

    yyparse();

    /* CLOSE ALL FILES */
    fclose(tree_file);
    fclose(sym_file);
    fclose(usage_file);

    printf("Phase 2 Complete. Files generated: parse_tree.txt, symtab.txt, usage_log.txt\n");
    return 0;
}