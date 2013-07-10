#include "LispMachine.hpp"
#include "subroutine.hpp"
#include <iostream>
#define BUILTIN_OP(name)\
Lazy::SExpression* name(Lazy::LispState*ls,Lazy::SExpression* args){\
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
#define BIND_BUILTIN(name,ls) ls->setVariable(#name,new Lazy::Subroutine(name))
#define var(x) new Lazy::Variable(#x)
#define dot(x,y) Lazy::DottedPair::make(x,y)
#define num(x) new Lazy::Number(x)
Lazy::SExpression* let(Lazy::LispState*ls,Lazy::SExpression*args){
        if(!args){return nullptr;}
        Lazy::DottedPair* dp = (Lazy::DottedPair*) args;
        Lazy::Variable * vr = (Lazy::Variable*)dp->car();
        if(!dp->cdr())
                return nullptr;
        Lazy::SExpression * expr;
        ls->setVariable(vr->name, expr=dp->cdr()->car()->Evaluate(ls, nullptr));
        return expr;
}
int main(int argc, char const *argv[])
{
        Lazy::LispState * ls = new Lazy::LispState;
        BIND_BUILTIN(add, ls);
        BIND_BUILTIN(div, ls);
        BIND_BUILTIN(sub, ls);
        BIND_BUILTIN(mul, ls);
        BIND_BUILTIN(let, ls);
        using namespace Lazy;
        std::cout << ((Lazy::Number*)Lazy::eval(ls,
                dot(var(sub),dot(dot(var(add),dot(num(10),num(11))),num(21)))))->get() << '\n';
        std::cout << ((Lazy::Number*)Lazy::eval(ls,dot(var(let),dot(var(a),dot(dot(var(add),dot(num(3),num(2))),(DottedPair*)nullptr)))))->get() << '\n';
        return 0;
}