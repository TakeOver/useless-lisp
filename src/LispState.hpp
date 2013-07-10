#pragma once
#include <unordered_map>
#include <string>
namespace Lazy{
        class SExpression;
        class LispState{
                std::unordered_map<std::string, SExpression*> globals;
        public:
                SExpression* getVariable(const std::string&name){ return globals[name]; }
                void setVariable(const std::string& name, SExpression* se){ globals[name]=se; }
        };
}