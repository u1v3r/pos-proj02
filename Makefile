# Makefile pre proj02

PNAME=proj02
CC=gcc
CFLAGS=-Wall -ansi -g -O $(PNAME).c -o $(PNAME)


build:
	$(CC) $(CFLAGS)
clean:
	rm -f $(PNAME)
