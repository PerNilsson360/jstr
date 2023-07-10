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

#ifndef _XPATH_EXP_HH_
#define _XPATH_EXP_HH_

#include <memory>
#include <list>
#include <stdexcept>

#include "XpathData.hh"

class XpathExpr {
public:
	XpathExpr() = default;
	virtual ~XpathExpr() = default;
	virtual XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const = 0;
private:
};

class UnaryExpr {
public:
	UnaryExpr(const XpathExpr* e);
protected:
  std::unique_ptr<const XpathExpr> _e;
};

class BinaryExpr {
public:
	BinaryExpr(const XpathExpr* l, const XpathExpr* r);
protected:
  std::unique_ptr<const XpathExpr> _l;
  std::unique_ptr<const XpathExpr> _r;
};

class MultiExpr {
public:
	MultiExpr(const XpathExpr* e);
	virtual ~MultiExpr();
	void addFront(const XpathExpr* e);
	void addBack(const XpathExpr* e);
	const std::list<const XpathExpr*>& getExprs() const;
protected:
	std::list<const XpathExpr*> _exprs;
};

class StrExpr {
public:
	StrExpr(const std::string& s);
	const std::string& getString() const;
protected:
  std::string _s;
};

class Root : public XpathExpr {
public:
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const;
private:
};

class Path : public XpathExpr, public MultiExpr {
public:
	Path(const XpathExpr* e);
	XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
 private:
};

class Step : public XpathExpr, public StrExpr {
public:
	Step(const std::string& s, const std::list<const XpathExpr*>* preds);
	~Step();
	XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
	static Step* create(const std::string& axisName,
						const std::string& nodeTest,
						std::list<const XpathExpr*>* predicates);
protected:
	virtual void evalStep(const XpathData& d,
						  size_t pos,
						  bool firstStep,
						  const std::vector<Node>& nodeSet,
						  std::vector<Node>& result) const = 0;
	void evalFilter(std::vector<Node>& result) const;
	const std::list<const XpathExpr*>* _preds;
};

class AllStep : public Step {
public:
	AllStep(const std::string& s, const std::list<const XpathExpr*>* preds);
protected:
	void evalStep(const XpathData& d,
				  size_t pos,
				  bool firstStep,
				  const std::vector<Node>& nodeSet,
				  std::vector<Node>& result) const;
};

class ChildStep : public Step {
public:
	ChildStep(const std::string& s, const std::list<const XpathExpr*>* preds);
protected:
	void evalStep(const XpathData& d,
				  size_t pos,
				  bool firstStep,
				  const std::vector<Node>& nodeSet,
				  std::vector<Node>& result) const;
};

class ParentStep : public Step {
public:
	ParentStep(const std::string& s, const std::list<const XpathExpr*>* preds);
protected:
	void evalStep(const XpathData& d,
				  size_t pos,
				  bool firstStep,
				  const std::vector<Node>& nodeSet,
				  std::vector<Node>& result) const;
};

class SelfStep : public Step {
public:
	SelfStep(const std::string& s, const std::list<const XpathExpr*>* preds);
protected:
	void evalStep(const XpathData& d,
				  size_t pos,
				  bool firstStep,
				  const std::vector<Node>& nodeSet,
				  std::vector<Node>& result) const;
};

class Predicate : public XpathExpr, UnaryExpr {
public:
  Predicate(const XpathExpr* e);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Descendant : public XpathExpr {
public:
	Descendant(const Path* relPath);
	XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
	std::unique_ptr<const Path> _path;
};

class ContextItem : public XpathExpr {
public:
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Parent : public XpathExpr {
public:
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Literal : public XpathExpr, StrExpr {
public:
  Literal(const std::string& l);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Number : public XpathExpr {
public:
  Number(double d);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
  double _d;
};

class Fun : public XpathExpr {	// TODO StrExp
public:
	Fun(const std::string& name, const std::list<const XpathExpr*>* args);
	~Fun();
	XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
	void checkArgs(size_t expectedSize) const;
	std::string _name;
	const std::list<const XpathExpr*>* _args;
};

class Args : public XpathExpr, public MultiExpr {
public:
	Args(const XpathExpr* e);
	XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Union : public XpathExpr, BinaryExpr {
public:
  Union(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};
  
class Or : public XpathExpr, BinaryExpr {
public:
  Or(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class And : public XpathExpr, BinaryExpr {
public:
  And(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Eq : public XpathExpr, BinaryExpr {
public:
  Eq(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Ne : public XpathExpr, BinaryExpr {
public:
  Ne(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Lt : public XpathExpr, BinaryExpr {
public:
  Lt(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Gt : public XpathExpr, BinaryExpr {
public:
  Gt(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Le : public XpathExpr, BinaryExpr {
public:
  Le(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Ge : public XpathExpr, BinaryExpr {
public:
  Ge(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Plus : public XpathExpr, BinaryExpr {
public:
  Plus(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Minus : public XpathExpr, BinaryExpr {
public:
  Minus(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Mul : public XpathExpr, BinaryExpr {
public:
  Mul(const XpathExpr* l, const XpathExpr* r);
   XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Div : public XpathExpr, BinaryExpr {
public:
  Div(const XpathExpr* l, const XpathExpr* r);
  XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

class Mod : public XpathExpr, BinaryExpr {
public:
  Mod(const XpathExpr* l, const XpathExpr* r);
   XpathData eval(const XpathData& d, size_t pos, bool firstStep = false) const override;
private:
};

#endif
