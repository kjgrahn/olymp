# $Id: Makefile,v 1.1 2006-02-02 19:59:32 grahn Exp $

INSTALLBASE = /usr/local

all:

install: olymp olymp.1
	install -m555 olymp $(INSTALLBASE)/bin/
	install -m444 olymp.1 $(INSTALLBASE)/man/man1/

clean:
	true

love:
	@echo "not war?"
