#ifndef _UTILS_HH_
#define _UTILS_HH_

#include <list>
#include "Expr.hh"

inline
void
addIfUnique(std::vector<Node>& ns, const Node& node) {
    bool found(false);
    for (const Node& n : ns) {
        if (n == node) {
            found = true;
        }
    }
    if (!found) {
        ns.emplace_back(node);
    }
}

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
