%{ /* -*- C++ -*- */

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
	
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstdio>
# include <string>
# include "xpath10_driver.hh"
# include "xpath10_parser.hh"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.

# undef yywrap
# define yywrap() 1

// [28]   	ExprToken	   ::=   	'(' | ')' | '[' | ']' | '.' | '..' | '@' | ',' | '::'	
// | NameTest	
// | NodeType	
// | Operator	
// | FunctionName	
// | AxisName	
// | Literal	
// | Number	
// | VariableReference	
// [29]   	Literal	   ::=   	'"' [^"]* '"'	
// | "'" [^']* "'"	
// [30]   	Number	   ::=   	Digits ('.' Digits?)?	
// | '.' Digits	
// [31]   	Digits	   ::=   	[0-9]+	
// [32]   	Operator	   ::=   	OperatorName	
// | MultiplyOperator	
// | '/' | '//' | '|' | '+' | '-' | '=' | '!=' | '<' | '<=' | '>' | '>='	
// [33]   	OperatorName	   ::=   	'and' | 'or' | 'mod' | 'div'	
// [34]   	MultiplyOperator	   ::=   	'*'	
// [35]   	FunctionName	   ::=   	QName - NodeType	
// [36]   	VariableReference	   ::=   	'$' QName	
// [37]   	NameTest	   ::=   	'*'	
// | NCName ':' '*'	
// | QName	
// [38]   	NodeType	   ::=   	'comment'	
// | 'text'	
// | 'processing-instruction'	
// | 'node'	
// [39]   	ExprWhitespace	   ::=   	S
    
// The location of the current token.
static yy::location loc;
// to debug add debug to %option below
%}
%option noyywrap nounput batch noinput

id    [^0-9'"=!<>\-+*/|()\[\].,: \n\t][^'"=!<>+*/:()\[\],  \n\t"]*
digit [0-9]+
blank [ \t\n]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step ();
%}

{blank}+   loc.step ();
[\n]+      loc.lines (yyleng); loc.step ();
"="                  return yy::xpath10_parser::make_EQ(loc);
"!="                 return yy::xpath10_parser::make_NE(loc);
"<"                  return yy::xpath10_parser::make_LT(loc);
"<="                 return yy::xpath10_parser::make_LE(loc);
">"                  return yy::xpath10_parser::make_GT(loc);
">="                 return yy::xpath10_parser::make_GE(loc);
"-"                  return yy::xpath10_parser::make_MINUS(loc);
"+"                  return yy::xpath10_parser::make_PLUS(loc);
"*"                  return yy::xpath10_parser::make_STAR(yytext, loc);
"/"                  return yy::xpath10_parser::make_SLASH(loc);
"//"                 return yy::xpath10_parser::make_DOUBLE_SLASH(loc);
"|"                  return yy::xpath10_parser::make_BAR(loc);
"("                  return yy::xpath10_parser::make_LPAREN(loc);
")"                  return yy::xpath10_parser::make_RPAREN(loc);
"["                  return yy::xpath10_parser::make_LSQUARE(loc);
"]"                  return yy::xpath10_parser::make_RSQUARE(loc);
"."                  return yy::xpath10_parser::make_DOT(yytext, loc);
".."                 return yy::xpath10_parser::make_DOUBLE_DOT(yytext, loc);
","                  return yy::xpath10_parser::make_COMMA(loc);
"::"                 return yy::xpath10_parser::make_DOUBLE_COLON(loc);
"and"                return yy::xpath10_parser::make_AND(loc);
"or"                 return yy::xpath10_parser::make_OR(loc);
"div"                return yy::xpath10_parser::make_DIV(loc);
"mod"                return yy::xpath10_parser::make_MOD(loc);
"ancestor"	         return yy::xpath10_parser::make_ANCESTOR(yytext, loc);
"ancestor-or-self"   return yy::xpath10_parser::make_ANCESTOR_OR_SELF(yytext, loc);
"child"	             return yy::xpath10_parser::make_CHILD(yytext, loc);
"descendant"	     return yy::xpath10_parser::make_DESCENDANT(yytext, loc);
"descendant-or-self" return yy::xpath10_parser::make_DESCENDANT_OR_SELF(yytext, loc);
"following"	         return yy::xpath10_parser::make_FOLLOWING(yytext, loc);
"following-sibling"  return yy::xpath10_parser::make_FOLLOWING_SIBLING(yytext, loc);
"parent"	         return yy::xpath10_parser::make_PARENT(yytext, loc);
"preceding"	         return yy::xpath10_parser::make_PRECEDING(yytext, loc);
"preceding-sibling"  return yy::xpath10_parser::make_PRECEDING_SIBLING(yytext, loc);
"self"               return yy::xpath10_parser::make_SELF(yytext, loc);

([0-9]+("."[0-9]+)?)|"."[0-9]+ {
  errno = 0;
  double d = strtod(yytext, nullptr);
  if (errno == ERANGE) {
    driver.error (loc, "double is out of range");
  }
  return yy::xpath10_parser::make_NUMBER(d, loc);
}

{id} {
  return yy::xpath10_parser::make_IDENTIFIER(yytext, loc);
}

\"[^"]*\"                return yy::xpath10_parser::make_LITERAL(yytext, loc);
'[^']*'                  return yy::xpath10_parser::make_LITERAL(yytext, loc);
 . {
	std::string message("invalid character: [");
	message += yytext[0];
	message += "]";
	driver.error (loc, message);
 }
<<EOF>>                  return yy::xpath10_parser::make_END(loc);
%%

void
xpath10_driver::scan_begin ()
{
/*
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      error ("cannot open " + file + ": " + strerror(errno));
      exit (EXIT_FAILURE);
    }
*/
  yyin = fmemopen((void*)xpath.c_str(), xpath.length(), "r");
  if (yyin == nullptr) {
     error("can not fmemopen " + xpath + " : " + strerror(errno));
     exit (EXIT_FAILURE);
  }
}



void
xpath10_driver::scan_end ()
{
  fclose (yyin);
}

