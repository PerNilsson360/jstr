check_PROGRAMS = test
test_SOURCES = test.cc
test_LDADD = ../src/libnljp.a
TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I$(top_srcdir)/include

clean-local:
	-rm *~
