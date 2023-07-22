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

#ifndef _UTILS_HH_
#define _UTILS_HH_

#include <list>
#include <Jstr.hh>

#include "Expr.hh"

namespace Jstr {
namespace Xpath {

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

}
}

#endif
