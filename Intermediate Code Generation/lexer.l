%{
#include <stdio.h>
#include <stdlib.h>
%}

/* Definitions */
DIGIT    [0-9]+
ID       [a-zA-Z_][a-zA-Z0-9_]*

%%

"int"       { printf("KEYWORD_INT\n"); }
"if"        { printf("KEYWORD_IF\n"); }
"else"      { printf("KEYWORD_ELSE\n"); }
"while"     { printf("KEYWORD_WHILE\n"); }

{DIGIT}     { printf("NUMBER %s\n", yytext); }
{ID}        { printf("IDENTIFIER %s\n", yytext); }

"=="        { printf("OP_EQ\n"); }
"!="        { printf("OP_NE\n"); }
"<"         { printf("OP_LT\n"); }
">"         { printf("OP_GT\n"); }
"="         { printf("OP_ASSIGN\n"); }

"+"         { printf("OP_ADD\n"); }
"-"         { printf("OP_SUB\n"); }
"*"         { printf("OP_MUL\n"); }
"/"         { printf("OP_DIV\n"); }

"("         { printf("LPAREN\n"); }
")"         { printf("RPAREN\n"); }
"{"         { printf("LBRACE\n"); }
"}"         { printf("RBRACE\n"); }
";"         { printf("SEMICOLON\n"); }
","         { printf("COMMA\n"); }

[ \t\n\r]+  { /* Skip whitespace */ }
"//".* { /* Skip single-line comments */ }

.           { fprintf(stderr, "Lexical Error: Unknown character %s\n", yytext); }

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Could not open %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}