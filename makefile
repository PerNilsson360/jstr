# MIT license
#
# Copyright 2023 Per Nilsson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

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

Value.o: Value.hh Value.cc
	g++ -g -c Value.cc

Expr.o: Expr.hh Expr.cc
	g++ -g -c Expr.cc

test: lex.yy.c xpath10_parser.tab.cpp xpath10_parser.tab.h xpath10_driver.cc test.cc Value.o Expr.o Node.o nljp.o
	g++ -g -o test test.cc xpath10_parser.tab.cpp lex.yy.c xpath10_driver.cc Value.o Expr.o Node.o nljp.o

clean:
	rm test xpath10_parser.tab.cpp lex.yy.c xpath10_parser.tab.h xpath10_parser.output location.hh position.hh stack.hh Value.o Expr.o Node.o nljp.o *~

