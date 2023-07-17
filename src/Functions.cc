#include <sstream>

#include "Utils.hh"
#include "Functions.hh"

// Fun
struct LastFun : Fun {
    LastFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        double size = d.getNodeSet().size(); 
        return Value(size);
    }
};

struct PositionFun : Fun {
    PositionFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        double position = pos + 1;
        return Value(position);
    }
};

struct CountFun : Fun {
    CountFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return arg.getNodeSetSize();
    }
};

struct LocalNameFun : Fun {
    LocalNameFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return arg.getLocalName();
    }
};

struct StringFun : Fun {
    StringFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        // TODO support 0 args
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return Value(arg.getString());
    }
};

// No fun, my babe No fun. No fun to hang around feelin' that same old way.
// No fun to hang around freaked out for another day.
struct NotFun : Fun {
    NotFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 1);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return Value(!arg.getBool());
    }
};

struct TrueFun : Fun {
    TrueFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
        return Value(true);
    }
};

struct FalseFun : Fun {
    FalseFun(const std::string& name, const std::list<const Expr*>* args) :
        Fun(args) {
        checkArgs(name, 0);
    }
    Value eval(const Value& d, size_t pos, bool firstStep = false) const override {
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
    if (name == "last") {
        return new LastFun(name, args);
    } else if (name == "position") {
        return new PositionFun(name, args);
    } else if (name == "count") {
        return new CountFun(name, args);
    } else if (name == "local-name") {
        return new LocalNameFun(name, args);
    } else if (name == "string") {
        return new StringFun(name, args);
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

