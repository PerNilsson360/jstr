#include <cmath>
#include <sstream>
#include <iostream>

#include "Utils.hh"
#include "Functions.hh"

struct CurrentFun : Fun {
    CurrentFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        return e.getCurrent();
    }
};

struct LastFun : Fun {
    LastFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        double size = d.getNodeSet().size(); 
        return Value(size);
    }
};

struct PositionFun : Fun {
    PositionFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        double position = pos + 1;
        return Value(position);
    }
};

struct CountFun : Fun {
    CountFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->evalExpr(e, d, pos);
        return arg.getNodeSetSize();
    }
};

struct LocalNameFun : Fun {
    LocalNameFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsZeroOrOne(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        if (_args == nullptr || _args->empty()) {
            const Node& n = d.getNode(pos);
            return n.getLocalName();
        } else {
            std::list<const Expr*>::const_iterator i = _args->begin();
            Value arg = (*i)->evalExpr(e, d, pos);
            return arg.getLocalName();
        }
    }
};

// String functions
struct StringFun : Fun {
    StringFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsZeroOrOne(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        if (_args == nullptr || _args->empty()) {
            const Node& n = d.getNode(pos);
            return n.getString();
        } else {
            std::list<const Expr*>::const_iterator i = _args->begin();
            Value arg = (*i)->evalExpr(e, d, pos);
            return Value(arg.getString());
        }
    }
};

struct ConcatFun : Fun {
    ConcatFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsGe(name, 2);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::string s;
        for (const Expr* arg : *_args) {
            Value val = arg->evalExpr(e, d, pos);
            s += val.getString();
        }
        return Value(s);
    }
};

struct StartsWithFun : Fun {
    StartsWithFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 2);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value left = (*i)->evalExpr(e, d, pos);
        ++i;
        Value right = (*i)->evalExpr(e, d, pos);
        std::string l = left.getString();
        std::string r = right.getString();
        return Value(l.rfind(r, 0) == 0);
    }
};

struct ContainsFun : Fun {
    ContainsFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 2);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value left = (*i)->evalExpr(e, d, pos);
        ++i;
        Value right = (*i)->evalExpr(e, d, pos);
        std::string l = left.getString();
        std::string r = right.getString();
        return Value(l.find(r, 0) != std::string::npos);
    }
};

struct SubstringBeforeFun : Fun {
    SubstringBeforeFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 2);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value left = (*i)->evalExpr(e, d, pos);
        ++i;
        Value right = (*i)->evalExpr(e, d, pos);
        std::string l = left.getString();
        std::string r = right.getString();
        size_t p = l.find(r, 0);
        return p == std::string::npos ? Value() : Value(l.substr(0, p));
    }
};

struct SubstringAfterFun : Fun {
    SubstringAfterFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 2);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value left = (*i)->evalExpr(e, d, pos);
        ++i;
        Value right = (*i)->evalExpr(e, d, pos);
        std::string l = left.getString();
        std::string r = right.getString();
        size_t p = l.find(r, 0);
        return p == std::string::npos ? Value() : Value(l.substr(p + r.size()));
    }
};

struct SubstringFun : Fun {
    SubstringFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsTwoOrThree(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        size_t argsSize(_args->size()); 
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value str = (*i)->evalExpr(e, d, pos);
        ++i;
        Value position = (*i)->evalExpr(e, d, pos);
        double len(-1);
        if (argsSize > 2) {
            ++i;
            Value length = (*i)->evalExpr(e, d, pos);
            len = length.getNumber();
            if (std::isnan(len)) {
                return Value("");
            }
            if (std::isinf(len)) {
                argsSize = 2;
            }
        }            
        const std::string& s = str.getString();
        double p = position.getNumber();
        if (std::isnan(p) || std::isnan(-p) || std::isinf(p)) {
            return Value("");
        } else if (p <= 0) {
            p = 0;
            if (argsSize == 3) {
                int diff = 1 - p;
                len -= diff;
                if (len < 0) {
                    argsSize = 2;
                }
            }
        }
        return  argsSize == 2 ? Value(s.substr(round(p))) : Value(s.substr(round(p), round(len)));
    }
};

struct StringLengthFun : Fun {
    StringLengthFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsZeroOrOne(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        double l;
        if (_args == nullptr || _args->empty()) {
            const Node& n = d.getNode(pos);
            const std::string& s = n.getString();
            l = s.size();
        } else  {
            std::list<const Expr*>::const_iterator i = _args->begin();
            Value v = (*i)->evalExpr(e, d, pos);
            const std::string& s = v.getString();
            l = s.size();
        }
        return Value(l);            
    }
};

struct TranslateFun : Fun {
    TranslateFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 3);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value left = (*i)->evalExpr(e, d, pos);
        ++i;
        Value source = (*i)->evalExpr(e, d, pos);
        ++i;
        Value target = (*i)->evalExpr(e, d, pos);
        std::string l = left.getString();
        const std::string& s = source.getString();
        const std::string& t = target.getString();
        size_t translateSize = std::min(s.size(), t.size());
        for (size_t i = 0, size = l.size() ; i < size; i++) {
            size_t p = s.find(l[i]);
            if (p != std::string::npos && p < translateSize) {
                l[i] = t[p];
            }
        }
        return Value(l);
    }
};

// Number functions
struct NumberFun : Fun {
    NumberFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsZeroOrOne(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        if (_args == nullptr || _args->empty()) {
            const Node& n = d.getNode(pos);
            return n.getNumber();
        } else {
            std::list<const Expr*>::const_iterator i = _args->begin();
            Value v = (*i)->evalExpr(e, d, pos);
            return Value(v.getNumber());
        }
    }
};

struct SumFun : Fun {
    SumFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value v = (*i)->evalExpr(e, d, pos);
        double r(0);
        for (const Node& n : v.getNodeSet()) {
            const std::string& s = n.getString();
            if (s.empty()) {
                r = NAN;
                break;
            } else {
                try {
                    r += std::stod(s);
                } catch (const std::exception& e) {
                    r = NAN;
                    break;
                }
            }
        }
        return Value(r);
    }
};

struct FloorFun : Fun {
    FloorFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value v = (*i)->evalExpr(e, d, pos);
        return Value(floor(v.getNumber()));
    }
};

struct CeilingFun : Fun {
    CeilingFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value v = (*i)->evalExpr(e, d, pos);
        return Value(ceil(v.getNumber()));
    }
};

struct RoundFun : Fun {
    RoundFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value v = (*i)->evalExpr(e, d, pos);
        return Value(round(v.getNumber()));
    }
};

// Booelan Functions
struct BooleanFun : Fun {
    BooleanFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgsZeroOrOne(name);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        if (_args == nullptr || _args->empty()) {
            const Node& n = d.getNode(pos);
            return Value(n.getBool());
        } else {
            std::list<const Expr*>::const_iterator i = _args->begin();
            Value arg = (*i)->evalExpr(e, d, pos);
            return Value(!arg.getBool());
        }
    }
};

// No fun, my babe No fun. No fun to hang around feelin' that same old way.
// No fun to hang around freaked out for another day.
struct NotFun : Fun {
    NotFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->evalExpr(e, d, pos);
        return Value(!arg.getBool());
    }
};

struct TrueFun : Fun {
    TrueFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        return Value(true);
    }
};

struct FalseFun : Fun {
    FalseFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep = false) const override {
        return Value(false);
    }
};

// Fun
Fun::Fun(const std::list<const Expr*>* args) :
    _args(args) {
}

Fun::~Fun() {
    deleteExprs(_args);
}

 Fun*
Fun::create(const std::string& name, const std::list<const Expr*>* args) {
     if (name == "current") {
         return new CurrentFun(name, args);
     } else if (name == "last") {
         return new LastFun(name, args);
     } else if (name == "position") {
         return new PositionFun(name, args);
     } else if (name == "count") {
         return new CountFun(name, args);
     } else if (name == "local-name") {
         return new LocalNameFun(name, args);
     } else if (name == "string") {
         return new StringFun(name, args);
     } else if (name == "concat") {
         return new ConcatFun(name, args);
     } else if (name == "starts-with") {
         return new StartsWithFun(name, args);
     } else if (name == "contains") {
         return new StartsWithFun(name, args);
     } else if (name == "substring-before") {
         return new SubstringBeforeFun(name, args);
     } else if (name == "substring-after") {
         return new SubstringAfterFun(name, args);
     } else if (name == "substring") {
         return new SubstringFun(name, args);
     } else if (name == "string-length") {
         return new StringLengthFun(name, args);
     } else if (name == "translate") {
         return new TranslateFun(name, args);
     } else if (name == "number") {
         return new NumberFun(name, args);
     } else if (name == "sum") {
         return new SumFun(name, args);
     } else if (name == "floor") {
         return new FloorFun(name, args);
     } else if (name == "ceiling") {
         return new CeilingFun(name, args);
     } else if (name == "round") {
         return new RoundFun(name, args);
     } else if (name == "boolean") {
         return new BooleanFun(name, args);
     } else if (name == "not") {
         return new NotFun(name, args);
     } else if (name == "true") {
         return new TrueFun(name, args);
     } else if (name == "false") {
         return new FalseFun(name, args);
     } else {
         std::stringstream ss;
         ss << "Fun::eval unkown function: " << name;
         throw std::runtime_error(ss.str());
     }
 }

void
Fun::checkArgs(const std::string& name, size_t expectedSize) const {
    size_t nArgs = _args == nullptr ? 0 : _args->size();
    if (nArgs != expectedSize) {
        std::stringstream ss;
        ss << "Fun::checkArgs, wrong number arguments to function: " << name
           << " expected: "<< expectedSize << " actual: " << nArgs;
        throw std::runtime_error(ss.str());
    }
}

void
Fun::checkArgsZeroOrOne(const std::string& name) const {
    size_t nArgs = _args == nullptr ? 0 : _args->size();
    if (nArgs > 1) {
        std::stringstream ss;
        ss << "Fun::checkArgs, wrong number arguments to function: " << name
           << " expected: zero or one actual: " << nArgs;
        throw std::runtime_error(ss.str());
    }
}

void
Fun::checkArgsTwoOrThree(const std::string& name) const {
    size_t nArgs = _args == nullptr ? 0 : _args->size();
    if (!(nArgs == 2 || nArgs == 3)) {
        std::stringstream ss;
        ss << "Fun::checkArgs, wrong number arguments to function: " << name
           << " expected: two or three actual: " << nArgs;
        throw std::runtime_error(ss.str());
    }
}

void
Fun::checkArgsGe(const std::string& name, size_t expectedSize) const {
    size_t nArgs = _args == nullptr ? 0 : _args->size();
    if (nArgs < expectedSize) {
        std::stringstream ss;
        ss << "Fun::checkArgs, wrong number arguments to function: " << name
           << " expected greater than or equal to: "<< expectedSize << " actual: " << nArgs;
        throw std::runtime_error(ss.str());
    }
}
