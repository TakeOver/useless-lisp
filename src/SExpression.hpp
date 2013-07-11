#pragma once
#include "Types.hpp"
#include "LispState.hpp"
#include <string>
namespace Lazy{
        #define EVAL_ARGS LispState* ls, SExpression* se


        class SExpression{
        public:
                virtual ~SExpression(){}
                virtual SExpression* Evaluate(EVAL_ARGS/*some args here, context and etc | TODO*/) = 0;
                virtual Type type() = 0;
        };
        SExpression* eval(LispState*, SExpression*);

        class Quot: public SExpression{
                SExpression* value;
        public:
                virtual ~Quot(){}
                virtual SExpression* Evaluate(EVAL_ARGS) override { return value; }
                Quot(SExpression* value):value(value){}
                virtual Type type() override { return value?value->type():Type::NIL; }
        };

        class Number: public SExpression{
                double value;
        public:
                virtual ~Number(){}
                Number(decltype(value) value = 0.0):value(value){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { return this; }
                virtual Type type() override { return Type::NUMBER; }
                decltype(value)& get() { return value; }
        };

        class String: public SExpression{
                std::string value;
        public:
                virtual ~String(){}
                String(const decltype(value) & value = ""):value(value){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { return this; }
                virtual Type type() override { return Type::STRING; }
                decltype(value)& get() { return value; }
        };

        class Boolean: public SExpression{
                bool value;
        public:
                virtual ~Boolean(){}
                Boolean(bool value =false):value(value){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { return this; }
                virtual Type type() override { return Type::BOOLEAN; }
                decltype(value)& get() { return value; }
                operator bool(){ return value; }
        };

        class DottedPair: public SExpression{
                SExpression* value;
                DottedPair* next;
        public:
                virtual ~DottedPair(){}
                DottedPair(decltype(value) value = nullptr, decltype(next) next = nullptr):value(value),next(next){}
                static DottedPair* make(decltype(value) value, decltype(value) next){
                        return new DottedPair(value, new DottedPair(next,nullptr));
                }

                virtual SExpression* Evaluate(EVAL_ARGS) override { return eval(ls,this); }
                virtual Type type() override { return Type::DOT; }
                SExpression* car() { return value; }
                DottedPair* cdr() { return next; }
                DottedPair* append(SExpression* elem){
                        DottedPair* end = this->last();
                                end->next = new DottedPair(elem,nullptr);
                        return this;
                }
                DottedPair* last() { 
                        DottedPair* iter = this;
                        while(iter->next!=nullptr)iter = iter->next;
                        return iter;
                }
        };

        class Variable: public SExpression{
        public:
                const std::string name;
                VarRef * cache = nullptr;
                virtual ~Variable(){}
                Variable(const std::string& name, VarRef * cache = nullptr):name(name),cache(cache){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { 
                        return  (       cache?
                                                cache:
                                                cache=ls->getVariable(name))?
                                        cache->ref:
                                        nullptr; 
                }
                virtual Type type() override { return Type::VARIABLE; }
        };

        class Subroutine: public SExpression{
                SExpression* (*fun)(LispState*, DottedPair*);
        public:
                using subr = SExpression* (*)(LispState*,DottedPair*);
                virtual ~Subroutine(){}
                Subroutine(subr fun):fun(fun){}

                virtual SExpression* Evaluate(EVAL_ARGS) override {
                        if(!se || se->type()!=Type::DOT) 
                                se = new DottedPair();
                        return fun(ls,(DottedPair*)se);
                }

                virtual Type type() override { return Type::SUBR; }
        };

        class Expression: public SExpression{
                DottedPair* func;
                DottedPair* args;
                LispState *ctx;
        public:
                virtual ~Expression(){}
                Expression(DottedPair *func,DottedPair* args, LispState* ctx=nullptr):func(func),args(args),ctx(ctx){}

                virtual SExpression* Evaluate(EVAL_ARGS) override {
                        LispState * state = new LispState(ctx);
                        if(!se || se->type()!=Type::DOT){
                                perror("DottedPair expected as func asrgs list\n");
                                return nullptr;
                        }
                        auto iter = args, arge = (DottedPair*)se;
                        while(iter){
                                if(!arge){
                                        perror("Number of arguments mismath in Expression call(lesser)\n");
                                        delete state;
                                        return nullptr;
                                }
                                if(iter->car()->type()!=Type::VARIABLE){
                                        perror("Variable expected as function param\n");
                                        return nullptr;
                                }
                                state->setVariable(((Variable*)iter->car())->name,eval(ls,arge->car()),true);
                                iter = iter->cdr();
                                arge = arge->cdr();
                        }
                        if(arge){
                                perror("Number of arguments mismath in Expression call(greater)\n");
                                delete state;
                                return nullptr;
                        }
                        return eval(state,func);
                }
                virtual Type type() override { return Type::EXPR; }
        };
        class FExpression: public SExpression{
                DottedPair* func;
                DottedPair* args;
                LispState *ctx;
        public:
                virtual ~FExpression(){}
                FExpression(DottedPair *func,DottedPair* args, LispState* ctx=nullptr):func(func),args(args),ctx(ctx){}

                virtual SExpression* Evaluate(EVAL_ARGS) override {
                        LispState * state = new LispState(ctx);
                        if(se->type()!=Type::DOT){
                                perror("DottedPair expected as func asrgs list\n");
                                return nullptr;
                        }
                        auto iter = args, arge = (DottedPair*)se;
                        while(iter){
                                if(!arge){
                                        perror("Number of arguments mismath in FExpression call(lesser)\n");
                                        delete state;
                                        return nullptr;
                                }
                                if(!iter->car() || iter->car()->type()!=Type::VARIABLE){
                                        perror("Variable expected as function param\n");
                                        return nullptr;
                                }
                                state->setVariable(((Variable*)iter->car())->name,(arge->car()),true);
                                iter = iter->cdr();
                                arge = arge->cdr();
                        }
                        if(arge){
                                perror("Number of arguments mismath in FExpression call(greater)\n");
                                delete state;
                                return nullptr;
                        }
                        return eval(state,func);
                }
                virtual Type type() override { return Type::FEXPR; }
        };

        class Macro: public SExpression{
                DottedPair* func;
                DottedPair* args;
        public:
                virtual ~Macro(){}
                Macro(DottedPair *func,DottedPair* args):func(func),args(args){}

                virtual SExpression* Evaluate(EVAL_ARGS) override {
                        LispState * state = new LispState(ls);
                        if(!se || se->type()!=Type::DOT){
                                perror("DottedPair expected as Macro args list\n");
                                return nullptr;
                        }
                        auto iter = args, arge = (DottedPair*)se;
                        while(iter){
                                if(!arge){
                                        perror("Number of arguments mismath in Macro call(lesser)\n");
                                        return nullptr;
                                }
                                if(!iter->car() || iter->car()->type()!=Type::VARIABLE){
                                        perror("Variable expected as macro param\n");
                                        delete state;
                                        return nullptr;
                                }
                                state->setVariable(((Variable*)iter->car())->name,(arge->car()),true);
                                iter = iter->cdr();
                                arge = arge->cdr();
                        }
                        if(arge){
                                perror("Number of arguments mismath in Macro call(greater)\n");
                                delete state;
                                return nullptr;
                        }
                        state->setPassive();
                        return eval(state,func);
                }
                virtual Type type() override { return Type::MACRO; }
        };


}