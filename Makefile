# Makefile pre proj03
# ! na preklad treba pouzit gmake !

CC = gcc
CFLAGS = -Wall -ansi -pthread -g -O $(PNAME).c -o $(PNAME)
PNAME = proj02
unameOS := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(unameOS),SunOS)
	CFLAGS = -Wall -lpthread -ansi -g -O -o $(PNAME) $(PNAME).c
endif


build: $(PNAME).c
	$(CC) $(CFLAGS)

clean: $(PNAME).c
	rm -f $(PNAME)
