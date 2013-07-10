#pragma once
namespace Lazy{
        enum class Type{
                NIL = 0,
                NUMBER,
                STRING,
                QUOT,
                DOT,
                SUBR,
                FSUBR,
                EXPR,
                FEXPR,
                MACRO,
                VARIABLE,
                CONSTANT,
                BOOLEAN
        };
}