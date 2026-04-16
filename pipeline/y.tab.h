/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KEYWORD_INT = 258,             /* KEYWORD_INT  */
    KEYWORD_IF = 259,              /* KEYWORD_IF  */
    KEYWORD_ELSE = 260,            /* KEYWORD_ELSE  */
    KEYWORD_WHILE = 261,           /* KEYWORD_WHILE  */
    IDENTIFIER = 262,              /* IDENTIFIER  */
    NUMBER = 263,                  /* NUMBER  */
    OP_ADD = 264,                  /* OP_ADD  */
    OP_SUB = 265,                  /* OP_SUB  */
    OP_MUL = 266,                  /* OP_MUL  */
    OP_DIV = 267,                  /* OP_DIV  */
    OP_EQ = 268,                   /* OP_EQ  */
    OP_NE = 269,                   /* OP_NE  */
    OP_LT = 270,                   /* OP_LT  */
    OP_GT = 271,                   /* OP_GT  */
    OP_ASSIGN = 272,               /* OP_ASSIGN  */
    SEMICOLON = 273,               /* SEMICOLON  */
    COMMA = 274,                   /* COMMA  */
    LPAREN = 275,                  /* LPAREN  */
    RPAREN = 276,                  /* RPAREN  */
    LBRACE = 277,                  /* LBRACE  */
    RBRACE = 278,                  /* RBRACE  */
    LOWER_THAN_ELSE = 279          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define KEYWORD_INT 258
#define KEYWORD_IF 259
#define KEYWORD_ELSE 260
#define KEYWORD_WHILE 261
#define IDENTIFIER 262
#define NUMBER 263
#define OP_ADD 264
#define OP_SUB 265
#define OP_MUL 266
#define OP_DIV 267
#define OP_EQ 268
#define OP_NE 269
#define OP_LT 270
#define OP_GT 271
#define OP_ASSIGN 272
#define SEMICOLON 273
#define COMMA 274
#define LPAREN 275
#define RPAREN 276
#define LBRACE 277
#define RBRACE 278
#define LOWER_THAN_ELSE 279

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 29 "parser.y"

    char* str;

#line 119 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
