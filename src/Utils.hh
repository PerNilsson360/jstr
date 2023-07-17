#ifndef _UTILS_HH_
#define _UTILS_HH_

#include <list>
#include "Expr.hh"

inline
void
deleteExprs(const std::list<const Expr*>* l) {
    if (l != nullptr) {
        for (const Expr* e : *l) {
            delete e;
        }
        delete l;
    }
}

#endif
