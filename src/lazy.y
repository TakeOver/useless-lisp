%{
#include "Parser.h"
#include "Lexer.h"
 
int yyerror(yyscan_t scanner, const char *msg){
        printf("Error:%s\n",msg);
        return 0;
}
 
%}
 
%code requires { 
        #ifndef YY_TYPEDEF_YY_SCANNER_T
                #define YY_TYPEDEF_YY_SCANNER_T
                typedef void* yyscan_t;
        #endif 
}
 
%output  "Parser.c"
%defines "Parser.h"
 
%error-verbose 
%start S_expr;
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

  
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_NUMBER
%token ATOM TOKEN_DOT TOKEN_TRUE TOKEN_FALSE
 
%%
 
S_expr: list;

S_value: ATOM | TOKEN_NUMBER | dot_pair | list | quot | TOKEN_TRUE | TOKEN_FALSE;
;
dot_pair: TOKEN_LPAREN S_value TOKEN_DOT S_value TOKEN_RPAREN
;
quot: '`' S_value
;
list: TOKEN_LPAREN list_args TOKEN_RPAREN
;
list_args: | S_value | list_args S_value
;
%%
int main(){
        char* str = (char*) calloc(100,sizeof(char));
        str = gets(str);
        yyscan_t scanner;
        YY_BUFFER_STATE state;
        if (yylex_init(&scanner)) {
                // couldn't initialize
                return 1;
        }
 
        state = yy_scan_string(str,scanner);
        if(!state) return 1;
        if (yyparse(scanner)) {
                // error parsing
                return 1;
        }
 
        yy_delete_buffer(state, scanner);
 
        yylex_destroy(scanner);
 
}