# Copyright (C) 1995-1998, Index Data I/S 
# All rights reserved.
# Sebastian Hammer, Adam Dickmeiss
# $Id$

# C Compiler
#CC=cc
CC=gcc

# For YAZ libraries.
YAZDIR=../yaz-1.4pl2
LIBDIR=$(YAZDIR)/lib

#LIBMOSI=../../xtimosi/src/libmosi.a $(LIBDIR)/librfc.a

SHELL=/bin/sh
INCLUDE=-I$(YAZDIR)/include -I. -I../xtimosi/src
LIBINCLUDE=-L$(LIBDIR)
DEFS=$(INCLUDE) $(CDEFS)
LIB=$(LIBDIR)/libserver.a 
LIBS=$(LIBDIR)/libserver.a $(LIBDIR)/libasn.a $(LIBDIR)/libodr.a \
$(LIBDIR)/libcomstack.a $(LIBMOSI) $(LIBDIR)/libutil.a
CPP=$(CC) -E
PROG=jmarcserver
PROGO=jmarcserver.o
RANLIB=ranlib

# External library (Machine Independent)
#ELIBS=
ELIBS=-lsocket -lnsl

all: $(PROG)

$(PROG): $(PROGO)
	$(CC) $(CFLAGS) $(LIBINCLUDE) -o $(PROG) $(PROGO) $(LIBS) $(ELIBS)

alll:

.c.o:
	$(CC) -c $(DEFS) $(CFLAGS) $<

clean:
	rm -f *.[oa] *~ test core mon.out gmon.out errlist tst cli $(PROG)

depend: depend1

depend1:
	sed '/^#Depend/q' <Makefile >Makefile.tmp
	$(CPP) $(DEFS) -M *.c >>Makefile.tmp
	mv -f Makefile.tmp Makefile

depend2:
	$(CPP) $(DEFS) -M *.c >.depend	

#ifeq (.depend,$(wildcard .depend))
#include .depend
#endif

#Depend --- DOT NOT DELETE THIS LINE
