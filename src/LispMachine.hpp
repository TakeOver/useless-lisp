#pragma once
#include "SExpression.hpp"
namespace Lazy{
        class LispMachine{
                //TODO
        };
        SExpression* eval(LispState* ls,SExpression* se){
                if(!se)return se;
                auto ty = se->type();
                switch(ty){
                        using t = Type;
                        case t::DOT:{
                                DottedPair *dp = static_cast<DottedPair*>(se);
                                SExpression * fun = dp->car();
                                if(!fun)
                                        return nullptr;
                                auto fty = fun->type();
                                if(fty==Type::VARIABLE || fty == Type::CONSTANT)
                                        fun = fun->Evaluate(ls,nullptr);
                                if(!fun)
                                        return nullptr;
                                fty = fun->type();
                                if(fty!=Type::SUBR && fty!=Type::FSUBR && fty!=Type::EXPR && fty!=Type::FEXPR){
                                        //err;
                                        return nullptr;
                                }
                                return fun->Evaluate(ls,dp->cdr());
                        };break;
                        case t::NUMBER: case t::BOOLEAN: case t::STRING: case t::NIL: return se;
                        case t::QUOT: return se->Evaluate(ls,nullptr); 
                        case t::SUBR: case t::FSUBR: case t::EXPR: case t::FEXPR: case t::MACRO:return se;
                        case t::VARIABLE: case t::CONSTANT: return se->Evaluate(ls,nullptr); 
                }
                return nullptr;
        }
}