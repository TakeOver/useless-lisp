#pragma once
#include "SExpression.hpp"
namespace Lazy{
        static Boolean const ltrue (true);
        static Boolean const lfalse (false);
        DottedPair* cons(SExpression* lhs, SExpression * rhs) {
                lhs = lhs?lhs->Evaluate():nullptr;
                rhs = rhs?rhs->Evaluate():nullptr;
                if(!rhs){
                        return new DottedPair(lhs);
                }
                if(rhs->type() == Type::DOT){
                        return new DottedPair(lhs,static_cast<DottedPair*>(rhs));
                }
                return new DottedPair(lhs,new DottedPair(rhs,nullptr));
        }
        DottedPair* list(DottedPair* args) { return args; }

        const Boolean* atom(SExpression* val){
                if(!val)return nullptr;
                switch(val->type()){
                        using i = Type;
                        case i::NUMBER: case i::STRING: case i::NIL: case i::BOOLEAN: return &ltrue;
                        default: return &lfalse;
                }
                return nullptr;
        }
        const Boolean* eq(SExpression* lhs, SExpression* rhs){
                if(atom(lhs) && atom(rhs)){
                        if(lhs->type()!=rhs->type())
                                return &lfalse;
                        if(lhs->type() == Type::NUMBER){
                                return static_cast<Number*>(lhs)->get() == static_cast<Number*>(rhs)->get()?&ltrue:&lfalse;
                        }
                        if(lhs->type() == Type::STRING){
                                return static_cast<String*>(lhs)->get() == static_cast<String*>(rhs)->get()?&ltrue:&lfalse;
                        }
                        if(lhs->type() == Type::NIL){
                                return &ltrue;
                        }
                        if(lhs->type() == Type::BOOLEAN){
                                return static_cast<Boolean*>(lhs)->get() == static_cast<Boolean*>(rhs)->get()?&ltrue:&lfalse;
                        }
                }
                return &lfalse;
        }
        const Boolean* neq(SExpression* lhs, SExpression * rhs){
                return eq(lhs,rhs)==&ltrue?&lfalse:&ltrue;
        }
        const Boolean* null(SExpression* lhs){
                if(!lhs || lhs->type() == Type::NIL)return &ltrue;
                return &lfalse;
        }
}