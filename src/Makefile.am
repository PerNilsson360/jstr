BUILT_SOURCES = xpath10_parser.hh xpath10_parser.cc xpath10_scanner.cc location.hh position.hh stack.hh
CLEANFILES = xpath10_parser.hh xpath10_parser.cc xpath10_scanner.cc location.hh position.hh stack.hh xpath10_parser.output
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_YFLAGS = -t -v -d -Wno-yacc
bin_PROGRAMS = nljp
nljp_SOURCES = xpath10_parser.yy xpath10_scanner.ll Expr.cc Node.cc Value.cc xpath10_driver.cc nljp.cc test.cc
AM_LFLAGS = -olex.yy.c

clean-local:
	-rm *~
