BUILT_SOURCES = xpath10_parser.hh xpath10_parser.cc xpath10_scanner.cc location.hh position.hh stack.hh
CLEANFILES = xpath10_parser.hh xpath10_parser.cc xpath10_scanner.cc location.hh position.hh stack.hh xpath10_parser.output
AM_CPPFLAGS = -g -I$(top_srcdir)/include
AM_YFLAGS = -t -v -d -Wno-yacc
AM_LFLAGS = -olex.yy.c
lib_LIBRARIES = libnljp.a
libnljp_a_SOURCES = xpath10_parser.yy xpath10_scanner.ll xpath10_driver.cc Expr.cc Expression.cc Functions.cc Node.cc ObjectNode.cc ArrayNode.cc LeafNode.cc Value.cc Env.cc Document.cc Jstr.cc
bin_PROGRAMS = jstr jxp
jstr_SOURCES = JstrMain.cc $(libnljp_a_SOURCES)
jxp_SOURCES = JxpMain.cc $(libnljp_a_SOURCES)

clean-local:
	-rm *~
