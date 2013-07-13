#include "LispMachine.hpp"
#include "subroutine.hpp"
#include <iostream>
#include <vector>
namespace { using namespace Lazy;}
#define BUILTIN_OP(name)\
Lazy::SExpression* name(Lazy::LispState*ls,Lazy::DottedPair* args){\
        if(!args)\
                return nullptr;\
        if(args->type()!=Lazy::Type::DOT)\
                return nullptr;\
        if(!((Lazy::DottedPair*)args)->cdr())return nullptr;\
        return Lazy:: name(ls,((Lazy::DottedPair*)args)->car(),((Lazy::DottedPair*)args)->cdr()->car());}
BUILTIN_OP(add);
BUILTIN_OP(sub);
BUILTIN_OP(div);
BUILTIN_OP(mul);
BUILTIN_OP(gt_num);
BUILTIN_OP(lt_num);
BUILTIN_OP(ge_num);
BUILTIN_OP(le_num);
BUILTIN_OP(eq_num);
BUILTIN_OP(neq_num);
BUILTIN_OP(eq);
BUILTIN_OP(neq);
BUILTIN_OP(cons);

#define BIND_BUILTIN(name,ls) ls->setVariable(#name,new Lazy::Subroutine(name),true)
//#define var(x) new Lazy::Variable(#x)
#define dot(x,y) Lazy::DottedPair::make(x,y)
#define num(x) new Lazy::Number(x)
void _print(LispState*ls, SExpression* res){    
        if(!res){
                std::cout << "nil";
                return;
        } 
        Lazy::DottedPair* func = nullptr;
        switch(res->type()){
        case Lazy::Type::NUMBER:
                std::cout << ((Lazy::Number*)res)->get();
                break;
        case Lazy::Type::STRING:
                std::cout << ((Lazy::String*)res)->get();
                break;
        case Lazy::Type::BOOLEAN:
                std::cout << (((Lazy::Boolean*)res)->get()?"#t":"#f");
                break;
        case Lazy::Type::VARIABLE/*atom actually*/:
                std::cout << (((Lazy::Variable*)res)->name);        
                break;
        case Lazy::Type::DOT:
                {
                        std::cout << "( ";
                        auto iter = ((Lazy::DottedPair*)res);
                        while(iter){
                                _print(ls,iter->car());
                                std::cout << ' ';
                                iter = iter->cdr();
                        }
                        std::cout << ")";
                }
                break;
        
        case Lazy::Type::QUOT:
                std::cout << '\'';
                _print(ls,res->Evaluate(ls, nullptr));
                break;
        case Lazy::Type::NIL: std::cout << "nil"; break;
        case Lazy::Type::EXPR:
                func = ((Lazy::Expression*)res)->get();
                std::cout << "( lambda ";
        case Lazy::Type::FEXPR: 
                if(Lazy::Type::FEXPR == res->type()){
                        func = (((Lazy::FExpression*)res)->get());
                        std::cout << "( flambda ";
                }
        case Lazy::Type::MACRO:
                if(Lazy::Type::MACRO == res->type()){
                        func = (((Lazy::Macro*)res)->get());
                        std::cout << "( macro ";
                }
                {
                        _print(ls,func->car());
                        std::cout << ' ';
                        _print(ls,func->cdr()->car());
                        std::cout << " )";
                        delete func->cdr();
                        delete func;
                }
                break;
        case Lazy::Type::SUBR: std::cout << "(`subroutine)"; break;
        }
}
Lazy::SExpression* print(Lazy::LispState*ls,Lazy::DottedPair*args){
        Lazy::SExpression* res;
        while(args){
                res = eval(ls,args->car());
                if(!res){
                        std::cout << "nil\t";
                        args = args->cdr();
                        continue;
                }
                _print(ls,res);
                std::cout << '\t';
                args = args->cdr();
        }
        std::cout << std::endl;
        return res;
}

Lazy::SExpression* var(Lazy::LispState*ls,Lazy::DottedPair*args){
        if(!args){perror("Variable excpected in var expression\n"); return nullptr;}
        Lazy::DottedPair* dp = (Lazy::DottedPair*) args;
        Lazy::SExpression * vr = dp->car();
        if(!dp->car() ||  (vr->type()!=Lazy::Type::VARIABLE)){
                perror("Variable excpected in var expression\n");
                return nullptr;
        }
        if(!dp->cdr()){
                ls->setVariable(((Lazy::Variable*)vr)->name, nullptr,true);
                return nullptr;
        }
        Lazy::SExpression * expr;
        ls->setVariable(((Lazy::Variable*)vr)->name, expr=eval(ls,dp->cdr()->car()),true);
        return expr;
}
Lazy::SExpression* varf(Lazy::LispState*ls,Lazy::DottedPair*args){
        if(!args){perror("Variable excpected in var expression\n"); return nullptr;}
        Lazy::DottedPair* dp = (Lazy::DottedPair*) args;
        Lazy::SExpression * vr = dp->car();
        if(!dp->car()){
                perror("Variable excpected in var expression\n");
                return nullptr;
        }
        vr = (vr->Evaluate(ls,nullptr));
        if(!vr || vr->type()!=Lazy::Type::VARIABLE){
                perror("Variable expected in var expression\n");
                return nullptr;                        
        }

        if(!dp->cdr()){
                ls->setVariable(((Lazy::Variable*)vr)->name, nullptr,true);
                return nullptr;
        }
        Lazy::SExpression * expr;
        ls->setVariable(((Lazy::Variable*)vr)->name, expr=eval(ls,dp->cdr()->car()),true);
        return expr;
}

Lazy::SExpression* debuglocals(Lazy::LispState*ls,Lazy::DottedPair*args){
        ls->debugLocals();
        return nullptr;
}

Lazy::SExpression* setq(Lazy::LispState*ls,Lazy::DottedPair*args){
        auto vr = args->car();
        if(!vr || (vr->type()!=Lazy::Type::VARIABLE)){
                perror("Variable expected in setq expression\n");
                return nullptr;
        }
        auto val = args->cdr();
        if(!val){
                perror("Value excpected in setq expression\n");
                return nullptr;
        }
        auto value = val->car();
        if(!value){
                perror("Value excpected in setq expression\n");
                return nullptr;
        }
        ls->setVariable(((Lazy::Variable*)vr)->name, value=eval(ls,value));
        return value;
}
Lazy::SExpression* set(Lazy::LispState*ls,Lazy::DottedPair*args){
        auto vr = args->car();
        if(!vr){
                perror("Variable expected in setq expression\n");
                return nullptr;
        }
        vr = eval(ls,vr);
        if(!vr || vr->type()!=Lazy::Type::VARIABLE){
                perror("Variable expected in setq expression\n");
                return nullptr;                        
        }
        auto val = args->cdr();
        if(!val){
                perror("Value excpected in setq expression\n");
                return nullptr;
        }
        auto value = val->car();
        if(!value){
                perror("Value excpected in setq expression\n");
                return nullptr;
        }
        ls->setVariable(((Lazy::Variable*)vr)->name, value=eval(ls,value));
        return value;
}
SExpression* tonum(LispState* ls, DottedPair* args){
    auto tmp = args->car();
    if(!tmp)perror("Nullptr error in tonum subroutine\n");
    tmp = tmp->Evaluate(ls,nullptr);
    if(!tmp)perror("Nullptr error in tonum subroutine\n");
    if(tmp->type() == Type::NUMBER)return tmp;
    if(tmp->type() == Type::STRING) return new Number(strtold(((String*)tmp)->get().c_str(),nullptr));
    perror("Only string or number can be spec. to tonum subroutine\n");
    return nullptr;
}
Lazy::SExpression* exit(Lazy::LispState*ls,Lazy::DottedPair*args){
        delete ls;
        Lazy::Number * num;
        exit((int)((num=(Lazy::Number*)args->car())?num->get():0));
        return nullptr;
}
SExpression* read(LispState * ls, DottedPair* args){
    std::string str;
    std::getline(std::cin,str);
    return new String(str);
}
SExpression* lambda(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(args->car());
        if(!argslist){
                perror("Argslist expected in lambda expression\n");
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in lambda expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(args->cdr()->car());
        if(!body){
                perror("lambda body expected\n");
                return nullptr;
        }
        return new Expression(body, argslist,ls); // yeah, This creates clojure.

}
SExpression* lambdaf(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(eval(ls,(args->car())));
        if(!argslist){
                perror("Argslist expected in lambda expression\n");
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in lambda expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(eval(ls,args->cdr()->car()));
        if(!body){
                perror("lambda body expected\n");
                return nullptr;
        }
        return new Expression(body, argslist,ls); // yeah, This creates clojure.

}
SExpression* flambda(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(args->car());
        if(!argslist){
                perror("Argslist expected in lambda expression\n");
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in lambda expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(args->cdr()->car());
        if(!body){
                perror("lambda body expected\n");
                return nullptr;
        }
        return new FExpression(body, argslist,ls); // yeah, This creates clojure.

}
SExpression* flambdaf(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(eval(ls,(args->car())));
        if(!argslist){
                perror("Argslist expected in lambda expression\n");
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in lambda expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(eval(ls,args->cdr()->car()));
        if(!body){
                perror("lambda body expected\n");
                return nullptr;
        }
        return new FExpression(body, argslist,ls); // yeah, This creates clojure.

}
SExpression* macro(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(args->car());
        if(!argslist){
                perror("Argslist expected in macro expression\n");
                std::cerr << (int) args->car()->type()<< '\n';
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in macro expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(args->cdr()->car());
        if(!body){
                perror("macro body expected\n");
                return nullptr;
        }
        return new Macro(body, argslist);

}
SExpression* macrof(LispState * ls, DottedPair* args){
        auto argslist = dynamic_cast<DottedPair*>(args->car()->Evaluate(ls, nullptr));
        if(!argslist){
                perror("Argslist expected in macro expression\n");
                std::cerr << (int) args->car()->type()<< '\n';
                return nullptr;
        }
        if(!args->cdr()){
                perror(("Function body expected in macro expression\n"));
                return nullptr;
        }
        auto body = dynamic_cast<DottedPair*>(args->cdr()->car()->Evaluate(ls, nullptr));
        if(!body){
                perror("macro body expected\n");
                return nullptr;
        }
        return new Macro(body, argslist);

}
SExpression* list(LispState * ls, DottedPair* args){
        std::vector<SExpression*> tmp;
        while(args){
                tmp.push_back(eval(ls,args->car()));
                args = args->cdr();
        }
        DottedPair * res = nullptr;
        for(auto i = tmp.rbegin(), e = tmp.rend(); i!=e;++i){
                res = new DottedPair(*i,res);
        }
        return res;
}
SExpression* quote(LispState * ls, DottedPair* args){
        std::vector<SExpression*> tmp;
        while(args){
                tmp.push_back((args->car()));
                args = args->cdr();
        }
        DottedPair * res = nullptr;
        for(auto i = tmp.rbegin(), e = tmp.rend(); i!=e;++i){
                res = new DottedPair(*i,res);
        }
        return res;
}
SExpression* car(LispState * ls, DottedPair* args){
        auto arg = dynamic_cast<DottedPair*>(eval(ls,args->car()));
        if(!args){
                perror("DottedPair expected in car\n");
                return nullptr;
        }
        return arg->car();
}
SExpression* cdr(LispState * ls, DottedPair* args){
        auto arg = dynamic_cast<DottedPair*>(eval(ls,args->car()));
        if(!args){
                perror("DottedPair expected in cdr\n");
                return nullptr;
        }
        return arg->cdr();
}
SExpression* progn(LispState * ls, DottedPair* args){
        SExpression* res;
        while(args){
                res = eval(ls,args->car());
                args = args->cdr();
        }
        return res;
}
SExpression* cond(LispState * ls, DottedPair* args){
        while(args){
                auto cond_pair = dynamic_cast<DottedPair*>(args->car());
                if(!cond_pair){
                        perror("DottedPair expected in cond\n");
                        return nullptr;
                }
                SExpression* expr;
                auto val = dynamic_cast<Boolean*>(expr=eval(ls,cond_pair->car()));
                if((val && *val) || (val == nullptr && !*null(expr))){
                        /*true*/
                        return eval(ls,cond_pair->cdr()->car());
                }
                args = args->cdr();
        }
        return nullptr;
}
SExpression* condf(LispState * ls, DottedPair* args){
        while(args){
                auto cond_pair = dynamic_cast<DottedPair*>(eval(ls,args->car()));
                if(!cond_pair){
                        perror("DottedPair expected in cond\n");
                        return nullptr;
                }
                SExpression* expr;
                auto val = dynamic_cast<Boolean*>(expr=eval(ls,cond_pair->car()));
                if(*val || (val == nullptr && (expr))){
                        /*true*/
                        return eval(ls,cond_pair->cdr()->car());
                }
                args = args->cdr();
        }
        return nullptr;
}
SExpression* loop(LispState * ls, DottedPair* args){
        auto cond = args->car();
        auto _body = args->cdr();
        if(!cond || !_body){
                perror("Incorrect args in loop subr\n");
                return nullptr;
        }
        auto body = _body->car();
        SExpression * res = nullptr;
        while(true){
                auto condres = eval(ls,cond);
                if(!condres){
                        break;
                }
                auto boolcondres = dynamic_cast<Boolean*>(condres);
                if(!boolcondres){
                        break;
                }
                if(!*boolcondres){
                        break;
                }
                res = eval(ls,body);
        }
        return res;

}
SExpression* loopf(LispState * ls, DottedPair* args){
        auto cond = eval(ls,args->car());
        auto _body = args->cdr();
        if(!cond || !_body){
                perror("Incorrect args in loopf subr\n");
                return nullptr;
        }
        auto body = eval(ls,_body->car());
        if(!body){
                perror("Incorrect args in loopf subr\n");
        }
        SExpression * res = nullptr;
        while(true){
                auto condres = eval(ls,cond);
                if(!condres){
                        break;
                }
                auto boolcondres = dynamic_cast<Boolean*>(condres);
                if(!boolcondres){
                        break;
                }
                if(!*boolcondres){
                        break;
                }
                res = eval(ls,body);
        }
        return res;

}
extern SExpression* eval(LispState* ls, DottedPair* args);
#define BIND_BUILTIN_ALIAS(x,y,z) z->setVariable(#x,z->getVariable(#y)->ref,true)
void bind_builtin(Lazy::LispState *ls){
        BIND_BUILTIN(add, ls);
        BIND_BUILTIN(div, ls);
        BIND_BUILTIN(sub, ls);
        BIND_BUILTIN(mul, ls);
        BIND_BUILTIN(gt_num,ls);
        BIND_BUILTIN(lt_num,ls);
        BIND_BUILTIN(ge_num,ls);
        BIND_BUILTIN(le_num,ls);
        BIND_BUILTIN(eq_num,ls);
        BIND_BUILTIN(neq_num,ls);
        BIND_BUILTIN_ALIAS(>, gt_num,ls);
        BIND_BUILTIN_ALIAS(<, lt_num,ls);
        BIND_BUILTIN_ALIAS(<=, le_num,ls);
        BIND_BUILTIN_ALIAS(>=, gt_num,ls);
        BIND_BUILTIN_ALIAS(==, eq_num,ls);
        BIND_BUILTIN_ALIAS(!=, neq_num,ls);
        BIND_BUILTIN(eq,ls);
        BIND_BUILTIN(neq,ls);
        BIND_BUILTIN(cons,ls);
        BIND_BUILTIN_ALIAS(*, mul,ls);
        BIND_BUILTIN_ALIAS(/, div,ls);
        BIND_BUILTIN_ALIAS(+, add,ls);
        BIND_BUILTIN_ALIAS(-, sub,ls);
        BIND_BUILTIN(var, ls);
        BIND_BUILTIN(print, ls);
        BIND_BUILTIN(debuglocals, ls);
        BIND_BUILTIN(setq, ls);
        BIND_BUILTIN(set, ls);
        BIND_BUILTIN(exit, ls);
        BIND_BUILTIN(tonum, ls);
        BIND_BUILTIN(read, ls);    
        BIND_BUILTIN(quote, ls);         
        BIND_BUILTIN(eval, ls);       
        BIND_BUILTIN(cond, ls);      
        BIND_BUILTIN(lambda, ls);
        BIND_BUILTIN(macro, ls);
        BIND_BUILTIN(macrof, ls);
        BIND_BUILTIN(list, ls);
        BIND_BUILTIN(car, ls);
        BIND_BUILTIN(loop, ls);
        BIND_BUILTIN(loopf, ls);
        BIND_BUILTIN_ALIAS(loop!, loopf,ls);
        BIND_BUILTIN(cdr, ls);
        BIND_BUILTIN(progn, ls);
        BIND_BUILTIN_ALIAS(macro!, macrof,ls);
        BIND_BUILTIN(condf, ls);
        BIND_BUILTIN_ALIAS(cond!, condf,ls);
        BIND_BUILTIN_ALIAS(set!, setq,ls);
        BIND_BUILTIN(lambdaf, ls);
        BIND_BUILTIN_ALIAS(lambda!, lambdaf,ls);    
        BIND_BUILTIN(flambda, ls);
        BIND_BUILTIN(flambdaf, ls);
        BIND_BUILTIN_ALIAS(flambda!, lambdaf,ls);
        BIND_BUILTIN(varf, ls);
        BIND_BUILTIN_ALIAS(var!,varf,ls);
        extern SExpression* eval(LispState*,const char*);
        ls->setVariable("nil", nullptr);
        eval(ls,"(var defmacro (macro (name args body)(var! name(macro! args body))))\
                (defmacro defun (name args body)(var! name(lambda! args body)))\
                (defmacro defunf (name args body)(var! name(flambda! args body)))\
                (defmacro let (vars body)((lambda! (list (car vars)) body)(car (cdr vars))))\
                (defmacro if (c t e) (cond! (list c t) (list #t e)))");
}