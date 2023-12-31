%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define api.parser.class {xpath10_parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
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
#include <string>
#include "Expr.hh"
#include "Functions.hh"

class xpath10_driver;

inline std::string stripLiteral(const std::string& s) {
	size_t len = s.size();
	return len == 2 ?  ""  : s.substr(1, len - 2);
}

using namespace Jstr::Xpath;

}
// The parsing context.
%param { xpath10_driver& driver }
%locations
%initial-action
{
  // Initialize the initial location.
  // @$.begin.filename = @$.end.filename = &driver.file;
};
%define parse.error verbose
%code
{
# include "xpath10_driver.hh"
}
%define api.token.prefix {TOK_}
%token
  END  0             "end of file"
  EQ                 "="
  NE                 "!="
  LT                 "<"
  LE                 "<="
  GT                 ">"
  GE                 ">="
  MINUS              "-"
  PLUS               "+"
  STAR               "*"
  SLASH              "/"
  DOUBLE_SLASH       "//"
  BAR                "|"
  LPAREN             "("
  RPAREN             ")"
  LSQUARE            "["
  RSQUARE            "]"
  DOT                "."
  DOUBLE_DOT         ".."
  COMMA              ","
  DOUBLE_COLON       "::"
  DOLLAR             "$"
  AND                "and"
  OR                 "or"
  DIV                "div"
  MOD                "mod"
  ANCESTOR           "ancestor"	     
  ANCESTOR_OR_SELF   "ancestor-or-self" 
  CHILD              "child"	         
  DESCENDANT         "descendant"	     
  DESCENDANT_OR_SELF "descendant-or-self"
  FOLLOWING          "following"	     
  FOLLOWING_SIBLING  "following-sibling"
  PARENT             "parent"	         
  PRECEDING          "preceding"	     
  PRECEDING_SIBLING  "preceding-sibling"
  SELF               "self"             
;
%token <std::string> IDENTIFIER "identifier"
%token <std::string> LITERAL "literal"
%token <double> NUMBER "number"

%type  <std::string> STAR
%type  <std::string> DOT
%type  <std::string> DOUBLE_DOT
%type  <std::string> ANCESTOR           
%type  <std::string> ANCESTOR_OR_SELF   
%type  <std::string> CHILD              
%type  <std::string> DESCENDANT         
%type  <std::string> DESCENDANT_OR_SELF 
%type  <std::string> FOLLOWING          
%type  <std::string> FOLLOWING_SIBLING  
%type  <std::string> PARENT             
%type  <std::string> PRECEDING          
%type  <std::string> PRECEDING_SIBLING  
%type  <std::string> SELF               

%type  <Path*> LocationPath
%type  <Path*> AbsoluteLocationPath
%type  <Path*> RelativeLocationPath
%type  <Expr*> Step
%type  <std::string> NodeTest
%type  <std::list<const Expr*>*> Predicates
%type  <const Expr*> Predicate
%type  <Expr*> PredicateExpr
%type  <Path*> AbbreviatedAbsoluteLocationPath
%type  <Path*> AbbreviatedRelativeLocationPath
%type  <std::string> AbbreviatedStep
%type  <std::string> AxisSpecifier
%type  <std::string> AxisName
%type  <Expr*> Expr
%type  <Expr*> PrimaryExpr
%type  <Fun*> FunctionCall
%type  <std::list<const Expr*>*> Arguments
%type  <Expr*> UnionExpr
%type  <Expr*> PathExpr
%type  <Expr*> FilterExpr
%type  <Expr*> OrExpr
%type  <Expr*> AndExpr
%type  <Expr*> EqualityExpr
%type  <Expr*> RelationalExpr
%type  <Expr*> AdditiveExpr
%type  <Expr*> MultiplicativeExpr
%type  <Expr*> UnaryExpr
%type  <std::string> NameTest
%type  <std::string> FunctionName
%type  <Expr*> VariableReference

%printer { yyoutput << $$; } <*>;
%%
%start Xpath;

Xpath :
  Expr                                          { driver.result.reset($1); }

// [1]   	LocationPath	           ::=   	RelativeLocationPath	
//                                              | AbsoluteLocationPath	
LocationPath :
  RelativeLocationPath                          { $$ = $1; }	
| AbsoluteLocationPath                          { $$ = $1; }


// [2]   	AbsoluteLocationPath	   ::=   	'/' RelativeLocationPath?	
//                                              | AbbreviatedAbsoluteLocationPath	

AbsoluteLocationPath :
  "/"                                           { $$ = new Path(new Root()); }
| "/" RelativeLocationPath                      { $$ = $2;  $$->addFront(new Root());} 
| AbbreviatedAbsoluteLocationPath               { $$ = $1; };

// [3]   	RelativeLocationPath	   ::=   	Step	
//                                              | RelativeLocationPath '/' Step	
//                                              | AbbreviatedRelativeLocationPath
RelativeLocationPath :
  Step	                                        { $$ = new Path($1); } 
| RelativeLocationPath "/" Step	                { $$ = $1; $$->addBack($3); }
| AbbreviatedRelativeLocationPath               { $$ = $1; };


// [4] Step	                            ::=     AxisSpecifier NodeTest Predicate*	
//			                                    | AbbreviatedStep
Step :
  NodeTest                                      { $$ = Step::create("", $1);} 
| NodeTest Predicates                           { $$ = Step::create("", $1); $$->addPredicates($2); }
| AxisSpecifier NodeTest                        { $$ = Step::create($1, $2); }
| AxisSpecifier NodeTest Predicates             { $$ = Step::create($1, $2); $$->addPredicates($3); }
| AbbreviatedStep                               { $$ = Step::create("", $1); }

// [5] AxisSpecifier                    ::=     AxisName "::"	
//                                              | AbbreviatedAxisSpecifier
AxisSpecifier :
  AxisName "::"                                 { $$ = $1; }

// [6] AxisName	                        ::=     'ancestor'	
//                                              | 'ancestor-or-self'	
//                                              | 'attribute'	
//                                              | 'child'	
//                                              | 'descendant'	
//                                              | 'descendant-or-self'	
//                                              | 'following'	
//                                              | 'following-sibling'	
//                                              | 'namespace'	
//                                              | 'parent'	
//                                              | 'preceding'	
//                                              | 'preceding-sibling'	
//                                              | 'self'
AxisName :
  "ancestor"	                                 { $$ = $1; }
| "ancestor-or-self"	                         { $$ = $1; }
| "child"	                                     { $$ = $1; }
| "descendant"	                                 { $$ = $1; }
| "descendant-or-self"	                         { $$ = $1; }
//| "following"	                                 { $$ = $1; }
| "following-sibling"	                         { $$ = $1; }
| "parent"	                                     { $$ = $1; }
//| "preceding"	                                 { $$ = $1; }
//| "preceding-sibling"	                         { $$ = $1; }
| "self"                                         { $$ = $1; }

// [7] NodeTest	                          ::=    NameTest	
//                                               | NodeType '(' ')'	
//                                               | 'processing-instruction' '(' Literal ')'
NodeTest :
  NameTest	                                     { $$ = $1; }

// [8] Predicate	                      ::=    '[' PredicateExpr ']'	
Predicates:
  Predicate                                      { $$ = new std::list<const Expr*>(1, $1); }
| Predicate Predicates                           { $$ = $2; $$->push_front($1);  }

// [9] PredicateExpr	                  ::=    Expr
Predicate:
  "[" PredicateExpr "]"	                         { $$ = new Predicate($2); }

// [9] PredicateExpr	                  ::=    Expr
PredicateExpr:
  Expr                                           { $$ = $1; }

// [10] AbbreviatedAbsoluteLocationPath	  ::=  	 '//' RelativeLocationPath	
AbbreviatedAbsoluteLocationPath:
  "//" RelativeLocationPath	                     { $$ = $2; $$->addAbsoluteDescendant(); }

// [119 AbbreviatedRelativeLocationPath	  ::=  	 RelativeLocationPath '//' Step	
AbbreviatedRelativeLocationPath:
  RelativeLocationPath "//" Step	             { $$ = $1; $$->addRelativeDescendant($3); }

// [12] AbbreviatedStep	                  ::=    '.'	
//                                               | '..'	
AbbreviatedStep:
  "."                                            { $$ = $1; }
| ".."                                           { $$ = $1; }
  
// [13] AbbreviatedAxisSpecifier           ::=   "@"?

// [14] Expr	                           ::=   OrExpr	
Expr:
  OrExpr                                         { $$ = $1; };

//[15]  PrimaryExpr	                       ::=   VariableReference	
//                                               | '(' Expr ')'	
//                                               | Literal	
//                                               | Number	
//                                               | FunctionCall
PrimaryExpr :
  VariableReference                              { $$ = $1; }
| "(" Expr ")"	                                 { $$ = $2; }
| "literal"	                                     { $$ = new StringLiteral(stripLiteral($1)); }
| "number"	                                     { $$ = new NumericLiteral($1); }
| FunctionCall                                   { $$ = $1; }

//[16] FunctionCall	                       ::=   FunctionName "(" ( Argument ( "," Argument )* )? ")"
FunctionCall :
  FunctionName "(" ")"                           { $$ = Fun::create($1, nullptr); }
| FunctionName "(" Arguments ")"                 { $$ = Fun::create($1, $3); }

// [17]  Argument	                       ::=   Expr
Arguments :
  Expr                                           { $$ = new std::list<const Expr*>(1, $1); }
| Expr "," Arguments                             { $$ = $3; $$->push_front($1); }

// [18] UnionExpr	                       ::=   PathExpr	
//                                               | UnionExpr '|' PathExpr	
UnionExpr :
  PathExpr	                                     { $$ = $1; }
| UnionExpr "|" PathExpr	                     { $$ = new Union($1, $3); }

// [19] PathExpr	                       ::=   LocationPath	
//                                               | FilterExpr	
//                                               | FilterExpr '/' RelativeLocationPath	
//                                               | FilterExpr '//' RelativeLocationPath	
PathExpr :
  LocationPath	                                 { $$ = $1; }
| FilterExpr	                                 { $$ = $1; }
| FilterExpr "/" RelativeLocationPath	         { $3->addFront($1); $$ = $3; }
| FilterExpr "//" RelativeLocationPath	         { $$ = $3; $3->addRelativeDescendant(); $3->addFront($1); };

//[20] FilterExpr	                       ::=   PrimaryExpr	
//                                               | FilterExpr Predicate
FilterExpr :
  PrimaryExpr	                                 { $$ = $1; }
| FilterExpr Predicate                           { $$ = $1; $$->addPredicates(new std::list<const Expr*>(1, $2));};

// [21] OrExpr	                           ::=   AndExpr	
//                                               | OrExpr 'or' AndExpr	
OrExpr :
  AndExpr                                        { $$ = $1; }
| OrExpr "or" AndExpr	                         { $$ = new Or($1, $3); }

// [22] AndExpr	                           ::=   EqualityExpr	
//                                               | AndExpr 'and' EqualityExpr	
AndExpr	:
  EqualityExpr	                                 { $$ = $1; }
| AndExpr "and" EqualityExpr	                 { $$ = new And($1, $3); }

// [23] EqualityExpr	                   ::=   RelationalExpr	
//                                               | EqualityExpr '=' RelationalExpr	
//                                               | EqualityExpr '!=' RelationalExpr	
EqualityExpr :
  RelationalExpr	                             { $$ = $1; }
| EqualityExpr "=" RelationalExpr	             { $$ = new Eq($1, $3); }
| EqualityExpr "!=" RelationalExpr               { $$ = new Ne($1, $3); }

// [24] RelationalExpr	                   ::=   AdditiveExpr	
//                                               | RelationalExpr '<' AdditiveExpr	
//                                               | RelationalExpr '>' AdditiveExpr	
//                                               | RelationalExpr '<=' AdditiveExpr	
//                                               | RelationalExpr '>=' AdditiveExpr
RelationalExpr :
  AdditiveExpr	                                 { $$ = $1; }
| RelationalExpr "<" AdditiveExpr	             { $$ = new Lt($1, $3); }
| RelationalExpr ">" AdditiveExpr	             { $$ = new Gt($1, $3); }
| RelationalExpr "<=" AdditiveExpr	             { $$ = new Le($1, $3); }
| RelationalExpr ">=" AdditiveExpr               { $$ = new Ge($1, $3); }

// [25] AdditiveExpr	                   ::=   MultiplicativeExpr	
//                                               | AdditiveExpr '+' MultiplicativeExpr	
//                                               | AdditiveExpr '-' MultiplicativeExpr	
AdditiveExpr :
  MultiplicativeExpr	                         { $$ = $1; }
| AdditiveExpr "+" MultiplicativeExpr	         { $$ = new Plus($1, $3); }
| AdditiveExpr "-" MultiplicativeExpr	         { $$ = new Minus($1, $3); }

// [26] MultiplicativeExpr	               ::=   UnaryExpr	
//                                               | MultiplicativeExpr MultiplyOperator UnaryExpr	
//                                               | MultiplicativeExpr 'div' UnaryExpr	
//                                               | MultiplicativeExpr 'mod' UnaryExpr	
MultiplicativeExpr :
  UnaryExpr	                                     { $$ = $1; }
| MultiplicativeExpr "*" UnaryExpr	             { $$ = new Mul($1, $3); }
| MultiplicativeExpr "div" UnaryExpr	         { $$ = new Div($1, $3); }
| MultiplicativeExpr "mod" UnaryExpr	         { $$ = new Mod($1, $3); }

// [27] UnaryExpr	                       ::=   UnionExpr	
//                                               | '-' UnaryExpr
UnaryExpr :
  UnionExpr	                                     { $$ = $1; }
| "-" UnaryExpr                                  { $$ = new Minus($2, nullptr); }

// [35] FunctionName	                   ::=   QName - NodeType 	
FunctionName :
  "identifier"                                   { $$ = $1; };

// [36 ] VariableReference	               ::=  '$' QName	
VariableReference :
"$" "identifier"                                 { $$ = new VarRef($2); }

// [37] NameTest	                       ::=   '*'	
//                                               | NCName ':' '*'	
//                                               | QName	
NameTest :
  "*"                                            { $$ = $1; }
| "identifier"                                   { $$ = $1; }

// [38 ]NodeType	                       ::=   'comment'	
//                                               | 'text'	
//                                               | 'processing-instruction'	
//                                               | 'node'

%%
void
yy::xpath10_parser::error (const location_type& l,
                          const std::string& m)
{
  driver.error (l, m);
}
