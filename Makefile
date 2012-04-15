# Makefile pre proj02
# ! na preklad treba pouzit gmake !

CC = gcc
CFLAGS = -Wall -ansi -pthread -g -O $(PNAME).c -o $(PNAME)
PNAME = proj02
unameOS := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(unameOS),SunOS)
	CC = cc
	CFLAGS = -mt -O -o $(PNAME) $(PNAME).c -lpthread
endif


build: $(PNAME).c
	$(CC) $(CFLAGS)

clean: $(PNAME).c
	rm -f $(PNAME)
