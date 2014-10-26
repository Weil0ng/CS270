#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-O2 -std=c99
OBJS=DiskEmulator.o FileSystem.o SuperBlock.o Utility.o

test: $(OBJS) Layer0Test Layer1INodeTest

Layer0Test: $(OBJS) Layer0Test.o
	$(CC) $(CFLAGS) -o $@ $^

Layer1INodeTest: $(OBJS) Layer1INodeTest.o
	$(CC) $(CFLAGS) -o $@ $^
    
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr Layer0Test Layer1INodeTest *.o
