%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {xpath10_parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
#include <iostream>
#include <string>
#include "XpathExpr.hh"  
class xpath10_driver;
inline std::string stripLiteral(const std::string& s) {
	size_t len = s.size();
	return len == 2 ?  ""  : s.substr(1, len - 2);
}
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
  END  0       "end of file"
  EQ           "="
  NE           "!="
  LT           "<"
  LE           "<="
  GT           ">"
  GE           ">="
  MINUS        "-"
  PLUS         "+"
  STAR         "*"
  SLASH        "/"
  DOUBLE_SLASH "//"
  BAR          "|"
  LPAREN       "("
  RPAREN       ")"
  LSQUARE      "["
  RSQUARE      "]"
  DOT          "."
  DOUBLE_DOT   ".."
  COMMA        ","
  AND          "and"
  OR           "or"
  DIV          "div"
  MOD          "mod"
;
%token <std::string> IDENTIFIER "identifier"
%token <std::string> LITERAL "literal"
%token <double> NUMBER "number"

%type  <std::string> STAR
%type  <std::string> DOT
%type  <std::string> DOUBLE_DOT

%type  <Path*> LocationPath
%type  <Path*> AbsoluteLocationPath
%type  <Path*> RelativeLocationPath
%type  <XpathExpr*> Step
%type  <std::string> NodeTest
%type  <std::list<const XpathExpr*>*> Predicates
%type  <const XpathExpr*> Predicate
%type  <XpathExpr*> PredicateExpr
%type  <Path*> AbbreviatedAbsoluteLocationPath
%type  <Path*> AbbreviatedRelativeLocationPath
%type  <std::string> AbbreviatedStep
%type  <XpathExpr*> Expr
%type  <XpathExpr*> PrimaryExpr
%type  <Fun*> FunctionCall
%type  <std::list<const XpathExpr*>*> Arguments
%type  <XpathExpr*> UnionExpr
%type  <XpathExpr*> PathExpr
%type  <XpathExpr*> FilterExpr
%type  <XpathExpr*> OrExpr
%type  <XpathExpr*> AndExpr
%type  <XpathExpr*> EqualityExpr
%type  <XpathExpr*> RelationalExpr
%type  <XpathExpr*> AdditiveExpr
%type  <XpathExpr*> MultiplicativeExpr
%type  <XpathExpr*> UnaryExpr
%type  <std::string> NameTest
%type  <std::string> FunctionName

%printer { yyoutput << $$; } <*>;
%%
%start Xpath;

Xpath :
  Expr                                          { driver.result.reset($1); }

LocationPath :
  RelativeLocationPath                          { $$ = $1; }	
| AbsoluteLocationPath                          { $$ = $1; }

AbsoluteLocationPath :
  "/"                                           { $$ = new Path(new Root()); }
| "/" RelativeLocationPath                      { static_cast<MultiExpr*>($2)->addFront(new Root()); $$ = $2; } 
| AbbreviatedAbsoluteLocationPath               { $$ = $1; };

RelativeLocationPath :
  Step	                                        { $$ = new Path($1); } 
| RelativeLocationPath "/" Step	                { $1->addBack($3); $$ = $1; }
| AbbreviatedRelativeLocationPath               { $$ = $1; };

Step :
/* AxisSpecifier*/
  NodeTest                                      { $$ = new Step($1, nullptr);} 
| NodeTest Predicates                           { $$ = new Step($1, $2); }    // Predicate*
| AbbreviatedStep                               { $$ = new Step($1, nullptr); }

//AxisSpecifier :
// AxisName "::"	
//AbbreviatedAxisSpecifier           {};

//  AxisName :
//   "ancestor"	                     
// | "ancestor-or-self"	
// | "attribute"	
// | "child"	
// | "descendant"	
// | "descendant-or-self"	
// | "following"	
// | "following-sibling"	
// | "namespace"	
// | "parent"	
// | "preceding"	
// | "preceding-sibling"	
// | "self"

NodeTest :
  NameTest	                                     { $$ = $1; }
//| NodeType "(" ")"                             {}
//| "processing-instruction" "(" Literal ")"     {}

Predicates:
  Predicate                                      { $$ = new std::list<const XpathExpr*>(1, $1); }
| Predicate Predicates                           { $2->push_front($1); $$ = $2; }

Predicate:
  "[" PredicateExpr "]"	                         { $$ = new Predicate($2); }

PredicateExpr:
  Expr                                           { $$ = $1; }

// AbbreviatedAbsoluteLocationPath	   ::=   	'//' RelativeLocationPath	
AbbreviatedAbsoluteLocationPath:
"//" RelativeLocationPath	                     { $$ = new Path(new Root()); $$->addBack(new Descendant($2)); }

// AbbreviatedRelativeLocationPath	   ::=   	RelativeLocationPath '//' Step	
AbbreviatedRelativeLocationPath:
  RelativeLocationPath "//" Step	             { $1->addBack(new Descendant(new Path($3))); $$ = $1; }

AbbreviatedStep:
  "."                                            { $$ = $1; }
| ".."                                           { $$ = $1; }
  
//AbbreviatedAxisSpecifier:
//  "@"?

Expr:
  OrExpr                                         { $$ = $1; };

PrimaryExpr :
//VariableReference
  "(" Expr ")"	                                 { $$ = $2; }
| "literal"	                                     { $$ = new Literal(stripLiteral($1)); }
| "number"	                                     { $$ = new Number($1); }
| FunctionCall                                   { $$ = $1; }

//[16]   	FunctionCall	   ::=   	FunctionName "(" ( Argument ( "," Argument )* )? ")"
FunctionCall :
  FunctionName "(" ")"                           { $$ = new Fun($1, nullptr);}
| FunctionName "(" Arguments ")"                 { $$ = new Fun($1, $3);}

//[17]   	Argument	   ::=   	Expr
Arguments :
  Expr                                           { $$ = new std::list<const XpathExpr*>(1, $1); }
| Expr "," Arguments                             { $3->push_front($1); $$ = $3; }

UnionExpr :
  PathExpr	                                     { $$ = $1; }
| UnionExpr "|" PathExpr	                     { $$ = new Union($1, $3); }

PathExpr :
  LocationPath	                                 { $$ = $1; }
| FilterExpr	                                 { $$ = $1; }
//| FilterExpr "/" RelativeLocationPath	         {}
//| FilterExpr "//" RelativeLocationPath	         {};

FilterExpr :
  PrimaryExpr	                                 { $$ = $1; }
//| FilterExpr Predicate                           {};


OrExpr :
  AndExpr                                        { $$ = $1; }
| OrExpr "or" AndExpr	                         { $$ = new Or($1, $3); }

AndExpr	:
  EqualityExpr	                                 { $$ = $1; }
| AndExpr "and" EqualityExpr	                 { $$ = new And($1, $3); }

EqualityExpr :
  RelationalExpr	                             { $$ = $1; }
| EqualityExpr "=" RelationalExpr	             { $$ = new Eq($1, $3); }
| EqualityExpr "!=" RelationalExpr               { $$ = new Ne($1, $3); }

RelationalExpr :
  AdditiveExpr	                                 { $$ = $1; }
| RelationalExpr "<" AdditiveExpr	             { $$ = new Lt($1, $3); }
| RelationalExpr ">" AdditiveExpr	             { $$ = new Gt($1, $3); }
| RelationalExpr "<=" AdditiveExpr	             { $$ = new Le($1, $3); }
| RelationalExpr ">=" AdditiveExpr               { $$ = new Ge($1, $3); }

AdditiveExpr :
  MultiplicativeExpr	                         { $$ = $1; }
| AdditiveExpr "+" MultiplicativeExpr	         { $$ = new Plus($1, $3); }
| AdditiveExpr "-" MultiplicativeExpr	         { $$ = new Minus($1, $3); }

MultiplicativeExpr :
  UnaryExpr	                                     { $$ = $1; }
| MultiplicativeExpr "*" UnaryExpr	             { $$ = new Mul($1, $3); }
| MultiplicativeExpr "div" UnaryExpr	         { $$ = new Div($1, $3); }
| MultiplicativeExpr "mod" UnaryExpr	         { $$ = new Mod($1, $3); }

UnaryExpr :
  UnionExpr	                                     { $$ = $1; }
| "-" UnaryExpr                                  { $$ = new Minus($2, nullptr); }

NameTest :
  "*"                                            { $$ = $1; }
| "identifier"                                   { $$ = $1; }
//| NCName ':' '*'	
//| QName                                        {};

// NodeType	   ::=
//   'comment'	
// | 'text'	
// | 'processing-instruction'	
// | 'node'


/*
Lexing 

ExprToken	   ::=   	'(' | ')' | '[' | ']' | '.' | '..' | '@' | ',' | '::'	
| NameTest	
| NodeType	
| Operator	
| FunctionName	
| AxisName	
| Literal	
| Number	
| VariableReference
	
Literal	   ::=   	'"' [^"]* '"'	
| "'" [^']* "'"	

Number	   ::=   	Digits ('.' Digits?)?	
| '.' Digits	

[Digits	   ::=   	[0-9]+ 

Operator	   ::=   	OperatorName	
| MultiplyOperator	
| '/' | '//' | '|' | '+' | '-' | '=' | '!=' | '<' | '<=' | '>' | '>='	

OperatorName	   ::=   	'and' | 'or' | 'mod' | 'div'	

MultiplyOperator	   ::=   	'*'	
*/

FunctionName :
  "identifier"                                    { $$ = $1; };

/*
VariableReference	   ::=   	'$' QName	
NameTest	   ::=   	'*'	
| NCName ':' '*'	
| QName	

NodeType	   ::=   	'comment'	
| 'text'	
| 'processing-instruction'	
| 'node'	

ExprWhitespace	   ::=   	S
 */

%%
void
yy::xpath10_parser::error (const location_type& l,
                          const std::string& m)
{
  driver.error (l, m);
}
