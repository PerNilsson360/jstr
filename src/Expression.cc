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

#include <sstream>
#include <stdexcept>

#include "xpath10_driver.hh"
#include <Jstr.hh>

namespace Jstr {
namespace Xpath {

Expression::Expression(const std::string& s) {
    xpath10_driver driver;
    if (driver.parse(s) != 0) {
        std::stringstream ss;
        ss << "Expression::Expression failed to parse exp: " << s;
        throw std::runtime_error(ss.str());
    }
    _expr = driver.result.release();
}

Expression::~Expression() {
    delete _expr;
    _expr = nullptr;
}

Value
Expression::eval(const Env& env) const {
    return _expr->eval(env, env.getCurrent(), 0);
}

}
}
