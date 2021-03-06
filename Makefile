# $Id: Makefile,v 1.1 2006-02-02 19:59:32 grahn Exp $

SHELL=/bin/bash
INSTALLBASE = /usr/local

all: olymp
all: seg
all: test/test

seg: seg.o libolymp.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ seg.o -L. -lolymp

olymp: olymp.o libolymp.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ olymp.o -L. -lolymp -lproj

libolymp.a: jfif.o
libolymp.a: tiff/tiff.o
libolymp.a: tiff/range.o
libolymp.a: exif.o
libolymp.a: wgs84.o
libolymp.a: sweref99.o
libolymp.a: transform.o
libolymp.a: gps.o
libolymp.a: metadata.o
libolymp.a: filename.o
	$(AR) -r $@ $^

CXXFLAGS=-Wextra -Wall -pedantic -std=c++14 -g -Os

.PHONY: check checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/libtest.a: test/hexread.o
test/libtest.a: test/endian.o
test/libtest.a: test/jfif.o
test/libtest.a: test/tiff.o
test/libtest.a: test/exif.o
test/libtest.a: test/gps.o
test/libtest.a: test/sweref99.o
test/libtest.a: test/transform.o
test/libtest.a: test/cluster.o
test/libtest.a: test/filename.o
	$(AR) -r $@ $^

test/%.o: CPPFLAGS+=-I.

test/test: test/test.o test/libtest.a libolymp.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test/test.o -Ltest/ -ltest -L. -lolymp -lproj

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
	$(RM) olymp seg
	$(RM) *.o tiff/*.o lib*.a
	$(RM) test/test test/test.cc test/*.o test/lib*.a
	$(RM) -r dep

love:
	@echo "not war?"

$(shell mkdir -p dep/{test,tiff})
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
dep/tiff/%.d: ;
dep/test/%.d: ;
-include dep/*.d
-include dep/tiff/*.d
-include dep/test/*.d
