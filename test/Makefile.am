check_PROGRAMS = test test_schematron small_xpath_example
test_SOURCES = test.cc
test_LDADD = $(top_srcdir)/src/libnljp.a
test_schematron_SOURCES = test_schematron.cc
test_schematron_LDADD = $(top_srcdir)/src/libnljp.a
small_xpath_example_SOURCES = small_xpath_example.cc
small_xpath_example_LDADD = $(top_srcdir)/src/libnljp.a
TESTS = $(check_PROGRAMS) test_schematron_1.sh test_schematron_2.sh test_schematron_3.sh 

AM_CPPFLAGS = -g -I$(top_srcdir)/include

clean-local:
	-rm *~
