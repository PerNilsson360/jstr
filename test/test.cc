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

#include <memory>
#include <cassert>
#include <iostream>
#include <Jstr.hh>


using namespace Jstr::Xpath;

void
testNumbers() {
    // NaN
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("'NO'", document));
        assert(std::isnan(r.getNumber()));
    }
    // +
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 + 2", document));
        assert(r.getNumber() == 3);
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("1 + /a", document));
        assert(r.getNumber() == 4);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b + /a/c", document));
        assert(r.getNumber() == 4);
        r = eval("/a/b+/a/c", document);
        assert(r.getNumber() == 4);
    }
    // -
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 - 2", document));
        assert(r.getNumber() == -1);
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("-1", document));
        assert(r.getNumber() == -1);
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("1 - /a", document));
        assert(r.getNumber() == -2);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b - /a/c", document));
        assert(r.getNumber() == 2);
        r = eval("/a/b - /a/c", document);
        assert(r.getNumber() == 2);
    }
    // *
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("3 * 2", document));
        assert(r.getNumber() == 6);
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("1 * /a", document));
        assert(r.getNumber() == 3);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b * /a/c", document));
        assert(r.getNumber() == 3);
        r= eval("/a/b * /a/c", document);
        assert(r.getNumber() == 3);
    }
    // div
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("4 div 2", document));
        assert(r.getNumber() == 2);
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("4 div 0", document));
        assert(std::isinf(r.getNumber()));
        assert(r.getString() == "Infinity");
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("-4 div 0", document));
        assert(std::isinf(r.getNumber()));
        assert(r.getString() == "-Infinity");
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("0 div 0", document));
        assert(std::isnan(r.getNumber()));
        assert(r.getString() == "NaN");
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("6 div /a", document));
        assert(r.getNumber() == 2);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b div /a/c", document));
        assert(r.getNumber() == 3);
    }
    // mod
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("5 mod 2", document));
        assert(r.getNumber() == 1);
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("5 mod -2", document));
        assert(r.getNumber() == 1);
    }
    // {
    //  nlohmann::json json;
    //  Value r(eval("5 mod -2", document));
    //  assert(r.getNumber() == -1); // TODO value is 1 should be -1 according to spec
    // }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("-5 mod -2", document));
        assert(r.getNumber() == -1);
    }
    // unary -
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("4 div (- 2)", document));
        assert(r.getNumber() == -2);
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("-6 div /a", document));
        assert(r.getNumber() == -2);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("-/a/b div /a/c", document));
        assert(r.getNumber() == -3);
    }
    // Sum
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("sum(/a)", document));
        assert(r.getNumber() == 3);
    }
    {
        // <a><b>1</b><b>2</b><b>3</b></a>
        const char* j = R"({"a":{"b":[1, 2, 3]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("sum(/a)", document));
        assert(r.getNumber() == 123);
        r =eval("sum(/a/b)", document);
        assert(r.getNumber() == 6);
    }
    // floor
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("floor(2.6)", document));
        assert(r.getNumber() == 2);
    }
    // celing
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("ceiling(2.6)", document));
        assert(r.getNumber() == 3);
    }
    // round
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("round(2.5)", document));
        assert(r.getNumber() == 3);
    }
}

void
testLogic() {
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("boolean(0)", document));
        assert(!r.getBoolean());
        r = eval("boolean(1)", document);
        assert(r.getBoolean());
        r =eval("boolean(number('foo'))", document);
        assert(!r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("true() and true()", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("true() and false()", document));
        assert(!r.getBoolean());
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a and 1", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("true() or true()", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("false() or false()", document));
        assert(!r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 or false()", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("'foo' or false()", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("not(false())", document));
        assert(r.getBoolean());
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("not(true())", document));
        assert(!r.getBoolean());
    }
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("not(/a and 1)", document));
        assert(!r.getBoolean());
    }
    {
        // <a><b>1</b><c>true</c><d>foo</d></a>
        const char* j = R"({"a":{"b":1,"c":true,"d":"foo"}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a and /a/b and /a/c and /a/d", document));
        assert(r.getBoolean());
    }
    // Union
    {
        // <a><b>1</b><c>true</c><d>foo</d></a>
        const char* j = R"({"a":{"b":1,"c":true,"d":"foo"}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b | /a/c", document));
        assert(r.getStringValue() == "1true");
        r = eval("/a/b | /a/c | /a/d", document);
        assert(r.getStringValue() == "1truefoo");
    }
}

void
testPaths() {
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"%@":{"&#":3,"?^":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/%@/&# | /%@/?^", document));
    }
    {
        // <a><b>1</b><b>2</b><b>3</b></a>
        const char* j = R"({"a":{"b":[1, 2, 3]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a/b)", document));
        assert(r.getNumber() == 3);
        r = eval("count(/a/b/ancestor::a)", document);
        assert(r.getNumber() == 1);
        r = eval("local-name(/a/b/ancestor::a)", document);
        assert(r.getString() == "a");        
        r = eval("count(/child::a/child::b)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/a/..)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/child::a/..)", document);
        assert(r.getNumber() == 1);
        r = eval("local-name(/a)", document);
        assert(r.getString() == "a");
        r = eval("local-name(/child::a)", document);
        assert(r.getString() == "a");
        r = eval("local-name(/a/.)", document);
        assert(r.getString() == "a");
        r = eval("local-name(/a/self::*)", document);
        assert(r.getString() == "a");
        r = eval("local-name(/a/self::a)", document);
        assert(r.getString() == "a");
        r = eval("count(/a/self::b)", document);
        assert(r.getNumber() == 0);
        r = eval("count(/a/child::*)", document);
        assert(r.getNumber() == 3);
    }
    {
        // <a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a)", document));
        assert(r.getNumber() == 1);
        r = eval("count(/a/b)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b/parent::a)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b/parent::*)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b/parent::c)", document);
        assert(r.getNumber() == 0);
        r = eval("count(/a/b/c)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b/c/e)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//e/ancestor::c)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//e/ancestor::b)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//e/ancestor::a)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//e/ancestor::*)", document);
        assert(r.getNumber() == 6);
        r = eval("count(/descendant::e)", document);
        assert(r.getNumber() == 2);
    }
    // * tests
    {
        // <a><b>1</b><c>true</c><d>foo</d></a>
        const char* j = R"({"a":{"b":1,"c":true,"d":"foo"}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/*)", document));
        assert(r.getNumber() == 1);
        r = eval("/*", document);
        assert(r.getString() == "1truefoo");
        r = eval("count(/a/*)", document);
        assert(r.getNumber() == 3);
        r = eval("/a/*", document);
        assert(r.getStringValue() == "1truefoo");
    }
    {
        // <a><b><b>1</b></b><b><b>2</b></b><b><c>3</c></b></a>
        const char* j = R"({"a":{"b": [{"b": 1},{"b": 2},{"c":3}]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a/*)", document));
        assert(r.getNumber() == 3);
        r = (eval("/a/*", document));
        assert(r.getStringValue() == "123");
        r = (eval("count(/a/b/*)", document));
        assert(r.getNumber() == 3);
        r = (eval("count(/a/b/following-sibling::*)", document));
        assert(r.getNumber() == 2);
        r = eval("/a/b/following-sibling::*", document);
        assert(r.getStringValue() == "23");
        r = eval("/a/b/following-sibling::*[2]", document);
        assert(r.getStringValue() == "3");
        r = eval("count(/a/b[b = 2]/following-sibling::*)", document);
        assert(r.getNumber() == 1);
        r = eval("/a/b[b = 2]/following-sibling::*", document);
        assert(r.getStringValue() == "3");
        r = (eval("count(/a/b/following-sibling::b)", document));
        assert(r.getNumber() == 2);
        r = (eval("/a/b/following-sibling::b", document));
        assert(r.getStringValue() == "23");
        r = eval("/a/b/following-sibling::b[1]", document);
        assert(r.getStringValue() == "2");
        r = eval("count(/a/b[b = 2]/following-sibling::b)", document);
        assert(r.getNumber() == 1);
        r = eval("/a/b[b = 2]/following-sibling::b", document);
        assert(r.getStringValue() == "3");
    }
    // Descendant tests
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//a)", document));
        assert(r.getNumber() == 1);
        r = eval("count(//*)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/descendant::*)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//.)", document);
        assert(r.getNumber() == 1);
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//a)", document));
        assert(r.getNumber() == 1);
        r = eval("count(/descendant::a)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//*)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/descendant::*)", document);
        assert(r.getNumber() == 3);
        r = eval("count(//.)", document);
        assert(r.getNumber() == 3);
        r = eval("count(//b)", document);
        assert(r.getNumber() == 1);
        r = eval("local-name(//b/..)", document);
        assert(r.getString() == "a");
        r = eval("count(//*)", document);
        assert(r.getNumber() == 3);
    
     }   
     {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//a)", document));
        assert(r.getNumber() == 1);
        r = eval("count(//b)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/descendant::b)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//b/c)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/descendant::b/c)", document);
        assert(r.getNumber() == 1);
        r = eval("count(//c)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/descendant::c)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//c/e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/descendant::c/e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/descendant::e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/a//e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/a/descendant::e)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//e/..)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/descendant::e/..)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//e/../../..)", document);
        assert(r.getNumber() == 1);
        r = eval("count(/descendant::e/../../..)", document);
        assert(r.getNumber() == 1);
    }
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//b)", document));
        assert(r.getNumber() == 4);
        r = eval("count(/descendant::b)", document);
        assert(r.getNumber() == 4);
        r = eval("count(//*)", document);
        assert(r.getNumber() == 5);
        r = eval("count(/descendant::*)", document);
        assert(r.getNumber() == 5);
        r = eval("count(//.)", document);
        assert(r.getNumber() == 5);
    }
    {
        // <a><a><a></a>1</a></a>
        const char* j = R"({"a":{"a":{"a":1}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//a)", document));
        assert(r.getNumber() == 3);
         r = eval("count(/descendant::a)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/a/descendant-or-self::a)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/a/descendant-or-self::*)", document);
        assert(r.getNumber() == 3);
        r = eval("count(//a/a)", document);
        assert(r.getNumber() == 2);
        r = eval("count(/descendant::a/a)", document);
        assert(r.getNumber() == 2);
        r = eval("count(//*)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/descendant::*)", document);
        assert(r.getNumber() == 3);
        r = eval("count(//.)", document);
        assert(r.getNumber() == 3);
        r = eval("count(/a/a/a/ancestor-or-self::a)", document);
        assert(r.getNumber() == 3);
    }
    {
        const char* j = R"({"a":[{"a":1},{"a":2},{"b":3}]})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(//a)", document));
        assert(r.getNumber() == 5);
        r = eval("count(/descendant::a)", document);
        assert(r.getNumber() == 5);
        r = eval("count(/a/descendant-or-self::a)", document);
        assert(r.getNumber() == 5);
        r = eval("count(//*)", document);
        assert(r.getNumber() == 6);
        r = eval("count(/descendant::*)", document);
        assert(r.getNumber() == 6);
        r = eval("count(//.)", document);
        assert(r.getNumber() == 6);
        r = eval("//a", document);
        assert(r.getStringValue() == "12312");
        r = eval("/descendant::a", document);
        assert(r.getStringValue() == "12312");
        r = eval("count(/a/a/ancestor-or-self::a)", document);
        assert(r.getNumber() == 4);
    }
    {
        // TODO replace above with following
        // <a><b><b>1</b></b><b><b>2</b></b><b><c>3</c></b></a>
    }
}

void
testRelations() {
    // =
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 = 1", document));
        assert(r.getBoolean());
        r = (eval("1 = 2", document));
        assert(!r.getBoolean());
        r = eval("true() = true()", document);
        assert(r.getBoolean());
        r = eval("false() = false()", document);
        assert(r.getBoolean());
        r = eval("true() = false()", document);
        assert(!r.getBoolean());
        r = eval("'a' = 'a'", document);
        assert(r.getBoolean());
        r = eval("\"a\" = \"a\"", document);
        assert(r.getBoolean());
        r = eval("'a' = 'b'", document);
        assert(!r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e = 1", document));
        assert(r.getBoolean());
        r = eval("/a/b/c/e = '1'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e = '2'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e = true()", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e = false()", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c = /a/d/c", document);
        assert(r.getBoolean());
        r = eval("/a/b/c = /a/d", document);
        assert(r.getBoolean());
        r = eval("/a/b/c=/a/d", document);
        assert(r.getBoolean());
    }
    // !=
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 != 1", document));
        assert(!r.getBoolean());
        r = (eval("1 != 2", document));
        assert(r.getBoolean());
        r = eval("true() != true()", document);
        assert(!r.getBoolean());
        r = eval("false() != false()", document);
        assert(!r.getBoolean());
        r = eval("true() != false()", document);
        assert(r.getBoolean());
        r = eval("'a' != 'a'", document);
        assert(!r.getBoolean());
        r = eval("\"a\" != \"a\"", document);
        assert(!r.getBoolean());
        r = eval("'a' != 'b'", document);
        assert(r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e != 1", document));
        assert(!r.getBoolean());
        r = eval("/a/b/c/e != '1'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e != '2'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e != true()", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e != false()", document);
        assert(r.getBoolean());
        r = eval("/a/b/c != /a/d/c", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c != /a/d", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c!=/a/d", document);
        assert(!r.getBoolean());
    }
    // <
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 < 1", document));
        assert(!r.getBoolean());
        r = (eval("1 < 2", document));
        assert(r.getBoolean());
        r = eval("true()  < true()", document);
        assert(!r.getBoolean());
        r = eval("false() < false()", document);
        assert(!r.getBoolean());
        r = eval("false() < true()", document);
        assert(r.getBoolean());
        r = eval("'a' < 1", document);
        assert(!r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e < 2", document));
        assert(r.getBoolean());
        r = eval("/a/b/c/e < '1'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e < '2'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e < true()", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e < false()", document);
        assert(!r.getBoolean());
        bool exception(false);
        try {
            r = eval("/a/b/c < /a/d/c", document);
        } catch (const std::runtime_error& e) {
            exception = true;
        }
        assert(exception);
    }
    // <=
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 <= 1", document));
        assert(r.getBoolean());
        r = (eval("1 <= 2", document));
        assert(r.getBoolean());
        r = eval("true()  <= true()", document);
        assert(r.getBoolean());
        r = eval("false() <= false()", document);
        assert(r.getBoolean());
        r = eval("true() <= false()", document);
        assert(!r.getBoolean());
        r = eval("'a' <= 1", document);
        assert(!r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e <= 2", document));
        assert(r.getBoolean());
        r = eval("/a/b/c/e <= '0.5'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e <= '2'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e <= true()", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e <= false()", document);
        assert(!r.getBoolean());
        bool exception(false);
        try {
            r = eval("/a/b/c <= /a/d/c", document);
        } catch (const std::runtime_error& e) {
            exception = true;
        }
        assert(exception);
    }
    // >
        {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 > 1", document));
        assert(!r.getBoolean());
        r = (eval("2 > 1", document));
        assert(r.getBoolean());
        r = eval("true()  > true()", document);
        assert(!r.getBoolean());
        r = eval("false() > false()", document);
        assert(!r.getBoolean());
        r = eval("true() > false()", document);
        assert(r.getBoolean());
        r = eval("'a' > 1", document);
        assert(!r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e > 2", document));
        assert(!r.getBoolean());
        r = eval("/a/b/c/e > '0.5'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e > '2'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e > true()", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e > false()", document);
        assert(r.getBoolean());
        bool exception(false);
        try {
            r = eval("/a/b/c > /a/d/c", document);
        } catch (const std::runtime_error& e) {
            exception = true;
        }
        assert(exception);
    }
    // >=
        {
        nlohmann::json json;
        Document document(json);
        Value r(eval("1 >= 1", document));
        assert(r.getBoolean());
        r = (eval("2 >= 1", document));
        assert(r.getBoolean());
        r = eval("true()  >= true()", document);
        assert(r.getBoolean());
        r = eval("false() >= false()", document);
        assert(r.getBoolean());
        r = eval("false() >= true()", document);
        assert(!r.getBoolean());
        r = eval("'a' >= 1", document);
        assert(!r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a/b/c/e >= 0.5", document));
        assert(r.getBoolean());
        r = eval("/a/b/c/e >= '2'", document);
        assert(!r.getBoolean());
        r = eval("/a/b/c/e >= '0.5'", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e >= true()", document);
        assert(r.getBoolean());
        r = eval("/a/b/c/e >= false()", document);
        assert(r.getBoolean());
        bool exception(false);
        try {
            r = eval("/a/b/c >= /a/d/c", document);
        } catch (const std::runtime_error& e) {
            exception = true;
        }
        assert(exception);
    }
}

void 
testFilter() {
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a/b[. = 1])", document));
        assert(r.getNumber() == 1);
        r = eval("count(/a/b[number() = 1])", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b[string() = '1'])", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b[boolean()])", document);
        assert(r.getNumber() == 4);
        r = eval("count(/a/b[not(. = 1)])", document);
        assert(r.getNumber() == 3);
        r = eval("count(/a/b[not(. = 1)][not(. = 2)])", document);
        assert(r.getNumber() == 2);
        r = eval("count(/a/b[not(. = 1)][not(. = 2)][not(. = 3)])", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/b[not(. = 1)][not(. = 2)][not(. = 3)][not(. = 4)])", document);
        assert(r.getNumber() == 0);
        r = eval("count(/a/b[count(//b) = 4])", document);
        assert(r.getNumber() == 4);
        r = eval("/a/b[1]", document);
        assert(r.getNumber() == 1);
        r = eval("/a/b[2]", document);
        assert(r.getNumber() == 2);
        r = eval("/a/b[2 + 1]", document);
        assert(r.getNumber() == 3);
        r = eval("/a/b[1 + 3]", document);
        assert(r.getNumber() == 4);
        r = eval("count(/a/b[0])", document);
        assert(r.getNumber() == 0);
        r = eval("count(/a/b[5])", document);
        assert(r.getNumber() == 0);

    }
    {
        //<a><b><c><e>1</e></c></b><d><f><e>1</e></f></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"f":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a/*[count(c) > 0])", document));
        assert(r.getNumber() == 1);
        r  = eval("count(/a/*/*[local-name(..) = 'b'])", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/*[count(*[local-name(.) = 'c']) > 0])", document);
        assert(r.getNumber() == 1);
    }
    {
        // <a><b><c><e>1</e></c></b><b><d><e>2</e></d></b></a>
        const char* j = R"({"a":{"b":[{"c":{"e":1}},{"d":{"e":2}}]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("count(/a/b[count(.//e) = 1])", document));
        assert(r.getNumber() == 2);
        r = eval("count(/a/b[count(//e) = 2])", document);
        assert(r.getNumber() == 2);
        r = eval("count(//*[local-name(.) = 'd'])", document);
        assert(r.getNumber() == 1);
        r = eval("count(/a/*[count(following-sibling::*) = 1])", document);
        assert(r.getNumber() == 1);
        r = eval("/a/*[count(following-sibling::*) = 1]", document);
        assert(r.getStringValue() == "1");
    }
    {
        nlohmann::json json;
        Document document(json);
        Value r(eval("(1 + 2)[. = 3]", document));
        assert(r.getBoolean()); 
    }
    {
        const char* j = R"({"a":{"b":[1, 2, 3, 4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);        
        Value r(eval("1[count(/a/b) = 4]", document));
        assert(r.getNumber() == 1);
    }
}

void
testNodeSetFunctions() {
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("/a[position()=1]", document));
        assert(r.getStringValue() == "3");
        r = eval("/a[position()=last()]", document);
        assert(r.getStringValue() == "3");
        r = eval("count(/a[position()=last()])", document);
        assert(r.getNumber() == 1);
        r = eval("local-name(/a[position()=last()])", document);
        assert(r.getStringValue() == "a");
        r = eval("count(/a[position()=0])", document);
        assert(r.getNumber() == 0);
        r = eval("count(/a[position()=2])", document);
        assert(r.getNumber() == 0);
    }
}

void
testStringFunctions() {
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("string(/)", document));
        assert(r.getString() == "3");
        assert(r.getStringValue() == "3");
        r = eval("string(/a)", document);
        assert(r.getString() == "3");
    }
    {
        // <a><b>1</b><c>2</c></a>
        const char* j = R"({"a":{"b":3,"c":1}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("string(/)", document));
        assert(r.getString() == "31");
        r = eval("string(/a)", document);
        assert(r.getString() == "31");
        r = eval("string(/a/b)", document);
        assert(r.getString() == "3");
        r = eval("string(/a/c)", document);
        assert(r.getString() == "1");
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("string(/)", document));
        assert(r.getString() == "11");
        r = (eval("string(/a)", document));
        assert(r.getString() == "11");
        r = (eval("string(/a/b)", document));
        assert(r.getString() == "1");
        r = (eval("string(/a/b)", document));
        assert(r.getString() == "1");
        r = (eval("string(/a/b/c)", document));
        assert(r.getString() == "1");
        r = (eval("string(/a/b/c/e)", document));
        assert(r.getString() == "1");
        r = (eval("string(/a/b/c/e/z)", document));
        assert(r.getString() == "");
    }
    {
        // <a><b>1</b><c>true</c><d>foo</d></a>
        const char* j = R"({"a":{"b":1,"c":true,"d":"foo"}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("string(/)", document));
        assert(r.getString() == "1truefoo");
        r = eval("string(/a)", document);
        assert(r.getString() == "1truefoo");
        r = eval("string(/a/b)", document);
        assert(r.getString() == "1");
        r = eval("string(/a/c)", document);
        assert(r.getString() == "true");
        r = eval("string(/a/d)", document);
        assert(r.getString() == "foo");
    }
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("string(/)", document));
        assert(r.getString() == "1234");
        r = eval("string(/a)", document);
        assert(r.getString() == "1234");
        r = eval("string(/a/b)", document);
        assert(r.getString() == "1");
        r = eval("string(//b)", document);
        assert(r.getString() == "1");
    }
    // Concat
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("concat(/, '')", document));
        assert(r.getString() == "1234");
        r = eval("concat(/, '5', 6)", document);
        assert(r.getString() == "123456");
    }
    // starts-with
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("starts-with(/, '')", document));
        assert(r.getBoolean());
        r = eval("starts-with(/, '12')", document);
        assert(r.getBoolean());
        r = eval("starts-with(/, '1234')", document);
        assert(r.getBoolean());
        r = eval("starts-with('foo', 'bar')", document);
        assert(!r.getBoolean());
    }
    // subsstring-before
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("substring-before(/, '4')", document));
        assert(r.getString() == "123");
        r = eval("substring-before(/, '1')", document);
        assert(r.getString() == "");
    }
    // subsstring-after
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("substring-after(/, '1')", document));
        assert(r.getString() == "234");
        r = eval("substring-after(/, '4')", document);
        assert(r.getString() == "");
    }
    // subsstring-after
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("substring-after(/, '1')", document));
        assert(r.getString() == "234");
        r = eval("substring-after(/, '4')", document);
        assert(r.getString() == "");
    }
    // substring
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value r(eval("substring(/, 2)", document));
        assert(r.getString() == "34");
        r = eval("substring(/, 4)", document);
        assert(r.getString() == "");
        r = eval("substring('12345', 2, 3)", document);
        assert(r.getString() == "345");
        r = eval("substring('12345', 1.5, 2.6)", document);
        assert(r.getString() == "345");
        r = eval("substring('12345', 0, 3)", document);
        assert(r.getString() == "12");
        r = eval("substring('12345', 0 div 0, 3)", document);
        assert(r.getString() == "");
        r = eval("substring('12345',1, 0 div 0)", document);
        assert(r.getString() == "");
        r = eval("substring('12345', -42)", document);
        assert(r.getString() == "12345");
        r = eval("substring('12345', -42, 1 div 0)", document);
        assert(r.getString() == "12345");
        r = eval("substring('12345', -1 div 0, 1 div 0)", document);
        assert(r.getString() == "");
    }
}

void
testStringValue() {
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Env env(document.getRoot());
        Value r(eval("/", document));
        assert(r.getStringValue() == "1234");
        r = eval("/a", document);
        assert(r.getString() == "1234");
        r = eval("/a/b", document);
        assert(r.getStringValue() == "1234");
        r = eval("//b", document);
        assert(r.getStringValue() == "1234");
    }
}

void
testEnv() {
    {
        // <a>3</a>
        const char* j = R"({"a":3})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Env env(document.getRoot());
        Expression e("/a");
        Value r = e.eval(env);
        assert(r.getNumber() == 3);
    }
    {
        // <a><b>1</b><b>2</b><b>3</b><b>4</b></a>
        const char* j = R"({"a":{"b":[1,2,3,4]}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value b1 = eval("/a/b[. = 1]", document);
        Env env(document.getRoot());
        env.addVariable("b1", b1);
        Expression e1("$b1");
        Value r(e1.eval(env));
        assert(r.getNumber() == 1);
        Expression e2("$b1 = current()/a/b[. = 1]");
        r = e2.eval(env);
        assert(r.getBoolean());
        Expression e3("$b1 = current()//b[. = 1]");
        r = e3.eval(env);
        assert(r.getBoolean());
        Value b4 = eval("/a/b[. = 4]", document);
        env.addVariable("b4", b4);
        Expression e4("$b1 < $b4");
        r = e4.eval(env);
        assert(r.getBoolean());
        Expression e5("($b1 + 3) = $b4");
        r = e5.eval(env);
        assert(r.getBoolean());
    }
    {
        //<a><b><c><e>1</e></c></b><d><c><e>1</e></c></d></a>
        const char* j = R"({"a":{"b":{"c":{"e":1}},"d":{"c":{"e":1}}}})";
        nlohmann::json json = nlohmann::json::parse(j);
        Document document(json);
        Value d = eval("//d", document);
        Env env(d);
        Expression e1("local-name(current()/c[count(//e) = 2])");
        Value r(e1.eval(env));
        assert(r.getString() == "c");
        Expression e2("count(current()//e)");
        r = e2.eval(env);
        assert(r.getNumber() == 1);
    }
}

int
main (int argc, char *argv[])
{
    testNumbers();
    testLogic();
    testPaths();
    testRelations();
    testFilter();
    testNodeSetFunctions();
    testStringFunctions();
    testStringValue();
    testEnv();
    return 0;
}
