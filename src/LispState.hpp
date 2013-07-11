#pragma once
#include <unordered_map>
#include <string>
#include "Types.hpp"
#include <iostream>
namespace Lazy{
        class SExpression;
        namespace {
                template<typename K, typename V> inline bool contains(const K& k,const V& v){ return k.find(v)!=k.end(); }
        }
        class LispState{
                std::unordered_map<std::string, VarRef*> globals;
                LispState * parent;
        public:
                void debugLocals(){
                        std::cout << "`( locals ";
                        for(auto &x:globals){
                                std::cout << x.first << " "; 
                        }
                        std::cout << ")\n";
                }
                LispState(LispState * parent = nullptr):parent(parent){}
                VarRef* getVariable(const std::string&name){
                        if(!contains(globals,name)){
                                return parent?parent->getVariable(name):nullptr;
                        } 
                        return globals[name]?globals[name]:nullptr; 
                }
                void setVariable(const std::string& name, SExpression* se, bool create = false){ 
                        if(contains(globals,name) || create){
                                if(!globals[name])
                                        globals[name] = new VarRef(se);
                                else if(globals[name]->mut) 
                                        globals[name]->ref = se;
                                else { /*error*/ }
                                return;
                        }
                        if(parent && ! parent->getVariable(name)){ 
                                globals[name]=new VarRef(se); 
                                return;
                        }
                        {/*error*/} 
                }
        };
}