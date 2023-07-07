CPPFLAGS=-I/usr/include/ -I../src -I../export -MMD -O3 -Wpedantic -Wall -Wextra -Werror
LDFLAGS =  -lstdc++

all: test

xpath10_parser.tab.cpp xpath10_parser.tab.h: xpath10_parser.yy
	bison -t -v -d xpath10_parser.yy -o xpath10_parser.tab.cpp --defines=xpath10_parser.tab.h

lex.yy.c: xpath10_scanner.ll xpath10_parser.tab.h
	flex xpath10_scanner.ll

nljp.o: nljp.hh nljp.cc
	g++ -g -c nljp.cc

Node.o: Node.hh Node.cc
	g++ -g -c Node.cc

XpathData.o: XpathData.hh XpathData.cc
	g++ -g -c XpathData.cc

XpathExpr.o: XpathExpr.hh XpathExpr.cc
	g++ -g -c XpathExpr.cc

test: lex.yy.c xpath10_parser.tab.cpp xpath10_parser.tab.h xpath10_driver.cc test.cc XpathData.o XpathExpr.o Node.o nljp.o
	g++ -g -o test test.cc xpath10_parser.tab.cpp lex.yy.c xpath10_driver.cc XpathData.o XpathExpr.o Node.o nljp.o

clean:
	rm test xpath10_parser.tab.cpp lex.yy.c xpath10_parser.tab.h xpath10_parser.output XpathData.o Node.o nljp.o

