// MIT license
//
// Copyright 2023 Per Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef _EXP_HH_
#define _EXP_HH_

#include <memory>
#include <list>
#include <stdexcept>

#include <Jstr.hh>

namespace Jstr {
namespace Xpath {

class Expr {
public:
    Expr();
    virtual ~Expr();
    Value eval(const Env& env, const Value& val, size_t pos, bool firstStep = false) const;
    virtual Value evalExpr(const Env& env,
                           const Value& val,
                           size_t pos,
                           bool firstStep = false) const = 0;
    void addPredicates(const std::list<const Expr*>* preds);
    const std::list<const Expr*>* takePredicates();
private:
    Value evalFilter(const Env& e, const Value& val) const;
    const std::list<const Expr*>* _preds;

};

class BinaryExpr {
public:
    BinaryExpr(const Expr* l, const Expr* r);
protected:
    std::unique_ptr<const Expr> _l;
    std::unique_ptr<const Expr> _r;
};

class MultiExpr {
public:
    MultiExpr(Expr* e);
    virtual ~MultiExpr();
    void addFront(Expr* e);
    void addBack(Expr* e);
    std::list<Expr*>& getExprs() ;
protected:
    std::list<Expr*> _exprs;
};

class StrExpr {
public:
    StrExpr(const std::string& s);
    const std::string& getString() const;
protected:
    std::string _s;
};

class Root : public Expr {
public:
    Root() = default;
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Path : public Expr, public MultiExpr {
public:
    Path(Expr* e);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
    void addAbsoluteDescendant();
    void addRelativeDescendant(Expr* Step);
    void addRelativeDescendant();
};

class Step : public Expr, public StrExpr {
public:
    Step(const std::string& s);
    static Expr* create(const std::string& axisName, const std::string& nodeTest);
    // TODO make this better
    static bool isAllStep(const Expr* step);
    static bool isSelfOrParentStep(const Expr* step);
protected:
};

class AllStep : public Step {
public:
    AllStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class AncestorStep : public Step {
public:
    AncestorStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class AncestorSelfStep : public AncestorStep {
public:
    AncestorSelfStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class ChildStep : public Step {
public:
    ChildStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class ParentStep : public Step {
public:
    ParentStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class SelfStep : public Step {
public:
    SelfStep(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class Predicate : public Expr {
public:
    Predicate(const Expr* e);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
private:
    std::unique_ptr<const Expr> _e; 
};

class DescendantAll : public Step { // TODO not really a step
public:
    DescendantAll();
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class DescendantOrSelfAll : public DescendantAll {
public:
    DescendantOrSelfAll();
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class DescendantSearch : public Step {
public:
    DescendantSearch(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class DescendantOrSelfSearch : public DescendantSearch {
public:
    DescendantOrSelfSearch(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class FollowingSiblingAll : public Step {
public:
    FollowingSiblingAll();
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class FollowingSiblingSearch : public Step {
public:
    FollowingSiblingSearch(const std::string& s);
    Value evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep = false) const override;
};

class Parent : public Expr {
public:
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Literal : public Expr, StrExpr {
public:
    Literal(const std::string& l);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Number : public Expr {
public:
    Number(double d);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
private:
    double _d;
};

class Args : public Expr, public MultiExpr {
public:
    Args(const Expr* e);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Union : public Expr, BinaryExpr {
public:
    Union(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};
  
class Or : public Expr, BinaryExpr {
public:
    Or(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class And : public Expr, BinaryExpr {
public:
    And(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Eq : public Expr, BinaryExpr {
public:
    Eq(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Ne : public Expr, BinaryExpr {
public:
    Ne(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Lt : public Expr, BinaryExpr {
public:
    Lt(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Gt : public Expr, BinaryExpr {
public:
    Gt(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Le : public Expr, BinaryExpr {
public:
    Le(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Ge : public Expr, BinaryExpr {
public:
    Ge(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Plus : public Expr, BinaryExpr {
public:
    Plus(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Minus : public Expr, BinaryExpr {
public:
    Minus(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Mul : public Expr, BinaryExpr {
public:
    Mul(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Div : public Expr, BinaryExpr {
public:
    Div(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class Mod : public Expr, BinaryExpr {
public:
    Mod(const Expr* l, const Expr* r);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

class VarRef : public Expr, StrExpr {
public:
    VarRef(const std::string& s);
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override;
};

}
}
#endif
