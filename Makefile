#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-Wall -O2 -std=c99
DEPS=GlobalParams.h DiskEmulator.h
OBJS=DiskEmulator.o

DiskEmulator: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr DiskEmulator *.o
