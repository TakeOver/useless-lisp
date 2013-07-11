%{
#include "utils.hpp"
   using namespace Lazy;
#include "Parser.h"
#include "Lexer.h"
int yyerror(yyscan_t scanner, const char *msg){
        printf("Error:%s\n",msg);
        return 0;
}
        LispState * ls;
SExpression * res = nullptr; 
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
%start prog;
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%union{
   Lazy::SExpression * expr;
   std::string* str;
}
  
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_NUMBER
%token ATOM TOKEN_DOT TOKEN_TRUE TOKEN_FALSE
%type<expr> S_value
%type<expr> S_expr prog
%type<expr> quot list list_args// dot_pair
%type<str> TOKEN_NUMBER ATOM

%%
 prog: S_expr { res = $1; $$ = $1;
        eval(ls,res);
} | prog  S_expr {
        res = $2; $$ = $2;
        eval(ls,res);
    }|;
S_expr: S_value {$$=$1;};

S_value:        ATOM            {$$=new Variable(*$1);}
|               TOKEN_NUMBER    { $$ = num(strtold($1->c_str(),nullptr));} 
//|               dot_pair        {$$=$1;}
|               list            {$$=$1;}
|               quot            {$$=$1;}
|               TOKEN_TRUE      { $$ = new Boolean(true);} 
|               TOKEN_FALSE     { $$ = new Boolean(false);}
;
//dot_pair: TOKEN_LPAREN S_value TOKEN_DOT S_value TOKEN_RPAREN { $$= dot($2,$4);}
//;
quot: '`' S_value { $$ = new Quot($2);}
;
list: TOKEN_LPAREN list_args TOKEN_RPAREN { $$ = $2; }
;
list_args:                      { $$ = nullptr;}
|       S_value list_args       { $$ = new Lazy::DottedPair($1,(DottedPair*)$2);}
;
%%
int main(){
        char* str = (char*) calloc(100,sizeof(char));
        str = gets(str);
        ls = new LispState;
        bind_builtin(ls);
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
