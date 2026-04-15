%{
#include "toy.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

"#".*              ;
"//".*             ;

"int"      { printf("INT "); return INT; }
"if"       { printf("IF "); return IF; }
"else"     { printf("ELSE "); return ELSE; }
"while"    { printf("WHILE "); return WHILE; }

[0-9]+     { printf("NUM "); yylval.num = atoi(yytext); return NUM; }

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("ID ");
    yylval.id = strdup(yytext);
    return ID;
}

"=="       { printf("EQ "); return EQ; }
"!="       { printf("NE "); return NE; }
"<"        { printf("LT "); return LT; }
">"        { printf("GT "); return GT; }

"+"        { printf("PLUS "); return PLUS; }
"-"        { printf("MINUS "); return MINUS; }
"*"        { printf("MUL "); return MUL; }
"/"        { printf("DIV "); return DIV; }

"="        { printf("ASSIGN "); return ASSIGN; }

";"        { printf("SEMICOLON\n"); return SEMICOLON; }
","        { printf("COMMA "); return COMMA; }

"("        { printf("LPAREN "); return LPAREN; }
")"        { printf("RPAREN "); return RPAREN; }

"{"        { printf("LBRACE\n"); return LBRACE; }
"}"        { printf("RBRACE\n"); return RBRACE; }

[ \t\n]    ;

. { printf("INVALID(%s) ", yytext); }

%%

int yywrap() { return 1; }
