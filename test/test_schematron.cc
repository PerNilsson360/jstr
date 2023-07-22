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

#include <iostream>
#include <Jstr.hh>


bool
eval(const std::string& schematron, const std::string& data, std::ostream& out) {
    nlohmann::json s = nlohmann::json::parse(schematron);
    nlohmann::json d = nlohmann::json::parse(data);
    return Jstr::Schematron::eval(s, d, out);
}

std::string
makeSchematron(const std::string& name,
               const std::string& context,
               const std::string& test,
               const std::string& message) {
    nlohmann::json assert;
    assert["test"] = test;
    assert["message"] = message;
    nlohmann::json rule;
    rule["assert"] = assert;
    rule["context"] = context;
    nlohmann::json pattern;
    pattern["rule"] = rule;
    pattern["name"] = name;
    nlohmann::json result;
    result["pattern"] = pattern;
    return result.dump();
}

void
test() {
    {
        // <a>3</a>
        const char* data = R"({"a":3})";
        const char* s1 = R"({"pattern" : [{"name":"test-a","rule":[{"context":"/","assert":[{"test":"count(a)=1","message": "number of a items must be 1"}]}]}]})";
        assert(eval(s1, data, std::cerr));
        const char* s2 = R"({"pattern" : [{"name":"test-a","rule":[{"context":"/","assert":[{"test":"count(a)=2","message": "number of a items must be 2"}]}]}]})";
        assert(!eval(s2, data, std::cerr));
    }
    {
        // <a><b>1</b><b>2</b><b>3</b></a>
        const char* data = R"({"a":{"b":[1, 2, 3]}})";
        std::string s1 = makeSchematron("test-b1", "/a/b", ". > 0", "all b > 0");
        assert(eval(s1, data, std::cerr));
        std::string s2 = makeSchematron("test-b2", "/a/b", ". < 3", "all b < 3");
        assert(!eval(s2, data, std::cerr));
    }
}

int
main (int argc, char *argv[])
{
    test();
    return 0;
}
