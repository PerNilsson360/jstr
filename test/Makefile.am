check_PROGRAMS = test test_schematron
test_SOURCES = test.cc
test_LDADD = $(top_srcdir)/src/libnljp.a
test_schematron_SOURCES = test_schematron.cc
test_schematron_LDADD = $(top_srcdir)/src/libnljp.a
TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -g -I$(top_srcdir)/include

clean-local:
	-rm *~
