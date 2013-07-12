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
                bool passive;
        public:
                void debugLocals(){
                        std::cout << "`( locals ";
                        for(auto &x:globals){
                                std::cout << x.first << " "; 
                        }
                        std::cout << ")\n";
                }
                void setPassive(){ passive = true; }
                LispState(LispState * parent = nullptr, bool passive = false):parent(parent),passive(passive){}
                VarRef* getVariable(const std::string&name){
                        if(!contains(globals,name)){
                                if(parent)
                                        return parent->getVariable(name);
                                return nullptr;
                        } 
                        return globals[name]; 
                }
                void setVariable(const std::string& name, SExpression* se, bool create = false){
                        if(passive && parent){
                                parent->setVariable(name, se,create);
                                return;
                        } 
                        if(contains(globals,name) || (create)){
                                if(!globals[name])
                                        globals[name] = new VarRef(se);
                                else if(globals[name]->mut) 
                                        globals[name]->ref = se;
                                else { /*error*/
                                        perror(("Variable:" + name + " is constant.\n").c_str());
                                }
                                return;
                        }
                        if((!parent || !parent->getVariable(name))){ 
                                globals[name]=new VarRef(se); 
                                return;
                        }
                        parent->setVariable(name, se);
                }
        };
}