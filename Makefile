#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-O2 -std=c99 -g
OBJS=DiskEmulator.o FileSystem.o INode.o SuperBlock.o Utility.o Directories.o

test: $(OBJS) Layer0Test Layer1CombinedTest

Layer0Test: $(OBJS) Layer0Test.o
	$(CC) $(CFLAGS) -o $@ $^

Layer1CombinedTest: $(OBJS) Layer1CombinedTest.o
	$(CC) $(CFLAGS) -o $@ $^
    
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr Layer0Test Layer1CombinedTest *.o
