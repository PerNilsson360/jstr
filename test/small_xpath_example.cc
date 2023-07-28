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

#include <cassert>
#include <Jstr.hh>

using namespace Jstr::Xpath;

int
main (int argc, char *argv[])
{
    nlohmann::json json = nlohmann::json::parse(R"({"a":{"b":1}})");
    // Create a document to manage memory for internal Node obejcts.
    Document document(json);
    // Search for a all the b nodes. It happens to be only one so
    // it can be used as a XPath context node.
    // Here we are using the simple eval interface.
    Value val = eval("//b", document);
    // Here we are using the more complex Expression, Env interface.
    // First "compile" a XPath expression.
    Expression exp("count(../b) = 1");
    // Create an Env object. It represents the XPath context.
    Env env(val);
    // Evaluate the expression given a XPath context.
    Value result = exp.eval(env);
    assert(result.getBoolean()); 
    return 0;
}
