# Makefile pre proj02

PNAME=proj02
CC=gcc
CCS=cc
SOLARIS=-mt -O -o $(PNAME) $(PNAME).c -lpthread
CFLAGS=-pthread -Wall -ansi -g -O $(PNAME).c -o $(PNAME)


linux:
	$(CC) $(CFLAGS)

solaris:
	$(CCS) $(SOLARIS)

clean:
	rm -f $(PNAME)
