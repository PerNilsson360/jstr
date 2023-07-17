#ifndef _FUNCTIONS_HH_
#define _FUNCTIONS_HH_

#include "Expr.hh"


class Fun : public Expr {
public:
    Fun(const std::list<const Expr*>* args);
    ~Fun();
    static Fun* create(const std::string& name, const std::list<const Expr*>* args);
protected:
    void checkArgs(const std::string& name, size_t expectedSize) const;
    void checkArgsGe(const std::string& name, size_t expectedSize) const;
    const std::list<const Expr*>* _args;
};

#endif
