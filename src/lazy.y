%{
#include "utils.hpp"
#include <vector>
   using namespace Lazy;
#include "Parser.h"
#include "Lexer.h"
int yyerror(std::vector<SExpression*>*,yyscan_t scanner, const char *msg){
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
%start prog;
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { std::vector<Lazy::SExpression*> *expression }

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
%type<expr> S_expr listnone
%type<expr> quot list list_args// dot_pair
%type<str> TOKEN_NUMBER ATOM

%%
prog:   S_expr          { expression->push_back($1);} 
|       prog  S_expr    { expression->push_back($2);}
|
;

S_expr: S_value {$$=$1;}
;

S_value:        ATOM            { $$ = new Variable(*$1); }
|               TOKEN_NUMBER    { $$ = num(strtold($1->c_str(),nullptr)); } 
//|               dot_pair        {$$=$1; }
|               list            { if ( !$1 ) $$ = new DottedPair( nullptr, nullptr ); else $$ = $1; }
|               quot            { $$ = $1; }
|               listnone        { $$ = $1; }
|               TOKEN_TRUE      { $$ = new Boolean( true ); } 
|               TOKEN_FALSE     { $$ = new Boolean( false ); }
;
//dot_pair: TOKEN_LPAREN S_value TOKEN_DOT S_value TOKEN_RPAREN { $$= dot($2,$4);}
//;
quot: '`' S_value { $$ = new Quot($2);}
;
listnone: TOKEN_LPAREN TOKEN_RPAREN { $$ = new DottedPair(nullptr,nullptr); }
list: TOKEN_LPAREN list_args TOKEN_RPAREN {$$ = $2; }
;
list_args:   S_value            { $$ = new DottedPair($1,nullptr);}
|       S_value list_args       { $$ = new Lazy::DottedPair($1,(DottedPair*)$2);}
;
%%
std::vector<SExpression*> eval_str(const char* str){ 
    yyscan_t scanner;
    std::vector<SExpression*> res;
    YY_BUFFER_STATE state;
    if (yylex_init(&scanner)) {
            // couldn't initialize
            return std::vector<SExpression*>();
    }
    state = yy_scan_string(str,scanner);
    if(!state) return std::vector<SExpression*>();
    if (yyparse(&res,scanner)) {
            // error parsing
            return std::vector<SExpression*>();
    }
    yy_delete_buffer(state, scanner);
    
 
    yylex_destroy(scanner);
    return res;
}
SExpression * eval_expr(LispState*ls, const std::vector<SExpression*>& exprs){
    SExpression* res = nullptr;
    for(auto&x:exprs)res = eval(ls,x);
    return res;
}
SExpression * eval(LispState *ls, const char*str){
    return eval_expr(ls,eval_str(str));
}
SExpression* eval(LispState* ls, DottedPair* args){
    auto tmp = args->car();
    if(!tmp)
        return nullptr;
    tmp = tmp->Evaluate(ls,nullptr);
    if(!tmp)
        return nullptr;
    if(tmp->type() == Type::DOT){
        return eval(ls,tmp);
    }
    if(tmp->type() == Type::STRING){
        return eval(ls, ((String*)tmp)->get().c_str());
    }
    return tmp->Evaluate(ls,nullptr);
}

int main(){
        LispState* ls = new LispState();
        bind_builtin(ls);
        std::vector<SExpression*>res;
        yyscan_t scanner;
        YY_BUFFER_STATE state;
        if (yylex_init(&scanner)) {
                // couldn't initialize
                return 1;
        }
        char* str = (char*) calloc(100,sizeof(char));
        strcpy(str,"(print ");
        while(true){
            memset(str+7,0,92);
            printf("repl>> ");
            gets(str+7);
            str[strlen(str)]=')';
            if(strcmp("(print )",str)==0)
                continue;
            state = yy_scan_string(str,scanner);
            if(!state) return 1;
            if (yyparse(&res,scanner)) {
                    // error parsing
                    return 1;
            }
            yy_delete_buffer(state, scanner);
            for(auto&x:res){
                eval(ls,x);
            }
            res.clear();
        }
 
        yylex_destroy(scanner);
}
