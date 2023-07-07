%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstdio>
# include <string>
# include "xpath10_driver.hh"
# include "xpath10_parser.tab.h"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.

# undef yywrap
# define yywrap() 1

// The location of the current token.
static yy::location loc;
// ro debug add debug to %option below
%}
%option noyywrap nounput batch noinput


id    [a-zA-Z][a-zA-Z_0-9\-]*
digit [0-9]+
blank [ \t]

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
"="      return yy::xpath10_parser::make_EQ(loc);
"!="     return yy::xpath10_parser::make_NE(loc);
"<"      return yy::xpath10_parser::make_LT(loc);
"<="     return yy::xpath10_parser::make_LE(loc);
">"      return yy::xpath10_parser::make_GT(loc);
">="     return yy::xpath10_parser::make_GE(loc);
"-"      return yy::xpath10_parser::make_MINUS(loc);
"+"      return yy::xpath10_parser::make_PLUS(loc);
"*"      return yy::xpath10_parser::make_STAR(yytext, loc);
"/"      return yy::xpath10_parser::make_SLASH(loc);
"//"     return yy::xpath10_parser::make_DOUBLE_SLASH(loc);
"|"      return yy::xpath10_parser::make_BAR(loc);
"("      return yy::xpath10_parser::make_LPAREN(loc);
")"      return yy::xpath10_parser::make_RPAREN(loc);
"["      return yy::xpath10_parser::make_LSQUARE(loc);
"]"      return yy::xpath10_parser::make_RSQUARE(loc);
"."      return yy::xpath10_parser::make_DOT(yytext, loc);
".."     return yy::xpath10_parser::make_DOUBLE_DOT(yytext, loc);
","      return yy::xpath10_parser::make_COMMA(loc);
"and"    return yy::xpath10_parser::make_AND(loc);
"or"     return yy::xpath10_parser::make_OR(loc);
"div"    return yy::xpath10_parser::make_DIV(loc);
"mod"    return yy::xpath10_parser::make_MOD(loc);

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

