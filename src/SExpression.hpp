#pragma once
#include "Types.hpp"
#include <string>
namespace Lazy{
        #define EVAL_ARGS


        class SExpression{
        public:
                virtual ~SExpression(){}
                virtual SExpression* Evaluate(EVAL_ARGS/*some args here, context and etc | TODO*/) = 0;
                virtual Type type();
        };

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

                virtual SExpression* Evaluate(EVAL_ARGS) override { /*TODO func call.*/ return nullptr; }
                virtual Type type() override { return Type::DOT; }
                decltype(value)& get() { return value; }
                SExpression* car() { return value; }
                DottedPair* cdr() { return next; }
                DottedPair* append(SExpression* elem){
                        DottedPair* end = this->last();
                        if(elem->type() == Type::DOT){
                                end->next = static_cast<DottedPair*>(elem);
                        }else{
                                end->next = new DottedPair(elem,nullptr);
                        }
                        return this;
                }
                DottedPair* get_next() const { return next; }
                DottedPair* get_next() { return const_cast<DottedPair*>(this)->next; }
                DottedPair* last() { 
                        DottedPair* iter = this;
                        while(iter->next!=nullptr)iter = iter->next;
                        return iter;
                }
        };

        class Variable: public SExpression{
                SExpression * value;
        public:
                virtual ~Variable(){}
                Variable(SExpression* value =nullptr):value(value){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { return value->Evaluate(); }
                virtual Type type() override { return Type::VARIABLE; }
                decltype(value)& get() { return value; }
                Variable * set(SExpression* value){ this->value = value; return this;}
        };
        
        class Constant: public SExpression{
                SExpression * value;
        public:
                virtual ~Constant(){}
                Constant(SExpression* value = nullptr):value(value){}

                virtual SExpression* Evaluate(EVAL_ARGS) override { return value->Evaluate(); }
                virtual Type type() override { return Type::CONSTANT; }
                decltype(value)& get() { return value; }
        };




}