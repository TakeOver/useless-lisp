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
#define var(x) new Lazy::Variable(#x)
#define dot(x,y) Lazy::DottedPair::make(x,y)
#define num(x) new Lazy::Number(x)

Lazy::SExpression* print(Lazy::LispState*ls,Lazy::DottedPair*args){
        Lazy::SExpression* res = eval(ls,args->car());
        if(!res){
                std::cout << "nil\n";
                return nullptr;
        }
        if(res->type() == Lazy::Type::NUMBER){
                std::cout << ((Lazy::Number*)res)->get() << '\n';
        }
        if(res->type() == Lazy::Type::STRING){
                std::cout << ((Lazy::String*)res)->get() << '\n';
        }
        if(res->type() == Lazy::Type::BOOLEAN){
                std::cout << (((Lazy::Boolean*)res)->get()?"#t":"#f") << '\n';
        }
        return res;
}

Lazy::SExpression* let(Lazy::LispState*ls,Lazy::DottedPair*args){
        if(!args){return nullptr;}
        Lazy::DottedPair* dp = (Lazy::DottedPair*) args;
        if(!dp->car() || dp->car()->type()!=Lazy::Type::VARIABLE)
                return nullptr;
        Lazy::Variable * vr = (Lazy::Variable*)dp->car();
        if(!dp->cdr()){
                ls->setVariable(vr->name, nullptr,true);
                return nullptr;
        }
        Lazy::SExpression * expr;
        ls->setVariable(vr->name, expr=eval(ls,dp->cdr()->car()),true);
        return expr;
}

Lazy::SExpression* debuglocals(Lazy::LispState*ls,Lazy::DottedPair*args){
        ls->debugLocals();
        return nullptr;
}

Lazy::SExpression* setq(Lazy::LispState*ls,Lazy::DottedPair*args){
        auto vr = args->car();
        if(!vr || vr->type()!=Lazy::Type::VARIABLE)
                return nullptr;
        auto val = args->cdr();
        if(!val){
                return nullptr;
        }
        auto value = val->car();
        if(!value)
                return nullptr;
        ls->setVariable(((Lazy::Variable*)vr)->name, value=eval(ls,value));
        return value;
}
SExpression* tonum(LispState* ls, DottedPair* args){
    auto tmp = args->car();
    if(!tmp)return nullptr;
    tmp = tmp->Evaluate(ls,nullptr);
    if(!tmp)return nullptr;
    if(tmp->type() == Type::NUMBER)return tmp;
    if(tmp->type() == Type::STRING) return new Number(strtold(((String*)tmp)->get().c_str(),nullptr));
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
        BIND_BUILTIN(let, ls);
        BIND_BUILTIN(print, ls);
        BIND_BUILTIN(debuglocals, ls);
        BIND_BUILTIN(setq, ls);
        BIND_BUILTIN(exit, ls);
        BIND_BUILTIN(tonum, ls);
        BIND_BUILTIN(read, ls);        
        BIND_BUILTIN(eval, ls);
        ls->setVariable("nil", nullptr);
}