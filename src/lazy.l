%{
 
        namespace Lazy{class SExpression;}
/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */
#include <string>
#include <vector>
#include <iostream>
#include "Parser.h"
 
#include <stdio.h>
 
%}
 
%option outfile="Lexer.c" header-file="Lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
 
LPAREN          "("
RPAREN          ")"
SEMICOLON       ";"
DOT             "."
TRUE            "#t"
FALSE           "#f"
QUOT            "'"
NUMBER          [0-9]+
WS              [ \r\n\t\f\v]+
ANY             [a-zA-Z\+\-\*\/\$\%\^\&\!\?\,\~\:\>\<\=]+
 
%%
{NUMBER}        { yylval->str = new std::string(yytext); return TOKEN_NUMBER; }
{TRUE}          { return TOKEN_TRUE; }
{FALSE}         { return TOKEN_FALSE; }
{ANY}           { yylval->str = new std::string(yytext);return ATOM;}
{SEMICOLON}     { int c; while((c = yyinput(yyscanner))!=EOF && c && c!='\n'); }
{WS}            { /* Skip blanks. */ } 
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{DOT}           { return TOKEN_DOT;}
{QUOT}          { return TOKEN_QUOT;}
.               {  }
 
%%
 
int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg); return 0;
}
