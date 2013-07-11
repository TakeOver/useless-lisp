#pragma once
namespace Lazy{
        enum class Type{
                NIL = 0,
                NUMBER,
                STRING,
                QUOT,
                DOT,
                SUBR,
                EXPR,
                MACRO,
                VARIABLE,
                BOOLEAN
        };
        class SExpression;
        struct VarRef{
                SExpression *ref = nullptr;
                bool mut = true;
                VarRef(SExpression*ref,bool mut = true):ref(ref),mut(mut){}
                VarRef(){}
        };
}