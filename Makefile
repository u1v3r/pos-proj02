# Makefile pre proj02

PNAME=proj02
CC=gcc
CCS=cc
SOLARIS=-mt -O -o $(PNAME) $(PNAME).c -lpthread
CFLAGS=-pthread -Wall -ansi -g -O $(PNAME).c -o $(PNAME)
uname_S:=$(shell sh -c 'uname -s 2>/dev/null || echo not')


build: $(PNAME).c
ifeq ($(uname_S),Linux)
	$(CC) $(CFLAGS)
endif
ifeq ($(OS),FreeBSD)
	$(CC) $(CFLAGS)
endif
ifeq ($(OS),SunOS)
	$(CCS) $(SOLARIS)
endif


clean: $(PNAME).c
	rm -f $(PNAME)
