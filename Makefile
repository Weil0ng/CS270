#This is the makefile to make the emulator
CC=gcc
LD=gcc

<<<<<<< HEAD
CFLAGS=-O2 -std=c99
OBJS=TestMain.o DiskEmulator.o Utility.o

TestMain: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
=======
CFLAGS=-Wall -O2 -std=c99
DEPS=GlobalParams.h DiskEmulator.h
OBJS=DiskEmulator.o DragonFS.o
>>>>>>> 90bc1498e264def910a8ac9396d424fa1853e4f0

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

DragonFS: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
<<<<<<< HEAD
	rm -fr TestMain *.o
=======
	rm -fr DragonFS *.o
>>>>>>> 90bc1498e264def910a8ac9396d424fa1853e4f0
