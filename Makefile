# $Id: Makefile,v 1.1 2006-02-02 19:59:32 grahn Exp $

SHELL=/bin/bash
INSTALLBASE = /usr/local

all: libolymp.a
all: test/test

libolymp.a: jfif.o
	$(AR) -r $@ $^

CFLAGS=-Wextra -Wall -pedantic -std=c99 -g -Os
CXXFLAGS=-Wextra -Wall -pedantic -std=c++14 -g -Os

.PHONY: check checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/libtest.a: test/hexread.o
test/libtest.a: test/jfif.o
	$(AR) -r $@ $^

test/%.o: CPPFLAGS+=-I.

test/test: test/test.o test/libtest.a libolymp.a
	$(CXX) $(CXXFLAGS) -o $@ test/test.o -Ltest/ -ltest -L. -lolymp

test/test.cc: test/libtest.a
	orchis -o $@ $^

.PHONY: install
install: olymp olymp.1
	install -m555 olymp $(INSTALLBASE)/bin/
	install -m444 olymp.1 $(INSTALLBASE)/man/man1/

.PHONY: tags
tags: TAGS
TAGS:
	etags *.{c,h,cc}

.PHONY: clean
clean:
	$(RM) *.o lib*.a
	$(RM) test/test test/test.cc test/*.o test/lib*.a
	$(RM) -r dep

love:
	@echo "not war?"

$(shell mkdir -p dep/test)
DEPFLAGS=-MT $@ -MMD -MP -MF dep/$*.Td
COMPILE.cc=$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.c=$(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	@mv dep/$*.{Td,d}

%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	@mv dep/$*.{Td,d}

dep/%.d: ;
dep/test/%.d: ;
-include dep/*.d
-include dep/test/*.d
