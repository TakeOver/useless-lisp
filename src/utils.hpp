#include "LispMachine.hpp"
#include "subroutine.hpp"
#include <iostream>
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
                std::cout << '`';
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
extern SExpression* eval(LispState* ls, DottedPair* args);
#define BIND_BUILTIN_ALIAS(x,y,z) z->setVariable(#x,z->getVariable(#y)->ref,true)
void bind_builtin(Lazy::LispState *ls){
        BIND_BUILTIN(add, ls);
        BIND_BUILTIN(div, ls);
        BIND_BUILTIN(sub, ls);
        BIND_BUILTIN(mul, ls);
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
        BIND_BUILTIN(eval, ls);      
        BIND_BUILTIN(lambda, ls);
        BIND_BUILTIN(macro, ls);
        BIND_BUILTIN(macrof, ls);
        BIND_BUILTIN_ALIAS(macro!, macrof,ls);
        BIND_BUILTIN_ALIAS(set!, setq,ls);
        BIND_BUILTIN(lambdaf, ls);
        BIND_BUILTIN_ALIAS(lambda!, lambdaf,ls);    
        BIND_BUILTIN(flambda, ls);
        BIND_BUILTIN(flambdaf, ls);
        BIND_BUILTIN_ALIAS(flambda!, lambdaf,ls);
        BIND_BUILTIN(varf, ls);
        BIND_BUILTIN_ALIAS(var!,varf,ls);
        ls->setVariable("nil", nullptr);
}