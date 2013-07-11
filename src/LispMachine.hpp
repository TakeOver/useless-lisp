#pragma once
#include "SExpression.hpp"
#include <iostream>
namespace Lazy{
        class LispMachine{
                //TODO
        };
        SExpression* eval(LispState* ls,SExpression* se){
                if(!se){std::cerr << "nulled se\n";return se;}
                auto ty = se->type();
                switch(ty){
                        using t = Type;
                        case t::DOT:{
                                DottedPair *dp = static_cast<DottedPair*>(se);
                                SExpression * fun = dp->car();
                                if(!fun)
                                        return nullptr;
                                auto fty = fun->type();
                                if(fty==Type::VARIABLE)
                                        fun = fun->Evaluate(ls,nullptr);
                                if(!fun)
                                        return nullptr;
                                fty = fun->type();
                                if(fty!=Type::SUBR && fty!=Type::EXPR){
                                        //err;
                                        return nullptr;
                                }
                                return fun->Evaluate(ls,dp->cdr());
                        };break;
                        case t::NUMBER: case t::BOOLEAN: case t::STRING: case t::NIL: return se;
                        case t::QUOT: return se->Evaluate(ls,nullptr); 
                        case t::SUBR: case t::EXPR:  case t::MACRO:return se;
                        case t::VARIABLE: return se->Evaluate(ls,nullptr); 
                }
                return nullptr;
        }
}