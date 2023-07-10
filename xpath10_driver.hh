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


#ifndef _XPATH10_DRIVER_HH_
#define _XPATH10_DRIVER_HH_
#include <string>
#include <memory>
#include "xpath10_parser.tab.h"
// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::xpath10_parser::symbol_type yylex (xpath10_driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;
// Conducting the whole scanning and parsing of Calc++.
class xpath10_driver
{
public:
  xpath10_driver ();
  virtual ~xpath10_driver ();

  // Handling the scanner.
  void scan_begin ();
  void scan_end ();
  // Run the parser on file F.
  // Return 0 on success.
  int parse(const std::string& xpath);
  // Used later to pass the file name to the location tracker.
  std::unique_ptr<const XpathExpr> result;
  std::string xpath;
  // Whether parser traces should be generated.
  bool trace_parsing;
  bool trace_scanning;
  // Error handling.
  void error (const yy::location& l, const std::string& m);
  void error (const std::string& m);
};
#endif // ! XPATH10_DRIVER_HH
