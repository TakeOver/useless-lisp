%{
 
/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */
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

NUMBER          [0-9]+
WS              [ \r\n\t\f\v]*
ANY             [a-zA-Z0-9\-?!~@#$%\^&\*\+\\]
 
%%
{NUMBER}        { return TOKEN_NUMBER; }
{TRUE}          { return TOKEN_TRUE; }
{FALSE}         { return TOKEN_FALSE; }
{ANY}           { return ATOM;}
({SEMICOLON}(.)*[\n])     {}
{WS}            { /* Skip blanks. */ } 
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{DOT}           { return TOKEN_DOT;}
.               {  }
 
%%
 
int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg); return 0;
}
