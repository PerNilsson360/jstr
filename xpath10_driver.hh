#ifndef XPATH10_DRIVER_HH
# define XPATH10_DRIVER_HH
# include <string>
# include <memory>
# include "xpath10_parser.tab.h"
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
