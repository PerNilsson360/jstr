#include "xpath10_driver.hh"
#include "xpath10_parser.tab.h"

xpath10_driver::xpath10_driver() :
  trace_scanning(false), trace_parsing(false) {
}

xpath10_driver::~xpath10_driver() {}

int
xpath10_driver::parse (const std::string& s) {
  xpath = s;
  scan_begin ();
  yy::xpath10_parser parser (*this);
  parser.set_debug_level (trace_parsing);
  int res = parser.parse ();
  scan_end ();
  return res;
}

void
xpath10_driver::error (const yy::location& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

void
xpath10_driver::error (const std::string& m) {
  std::cerr << m << std::endl;
}
