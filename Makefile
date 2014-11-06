#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-O2 -std=c99 -g
OBJS=Directories.o DiskEmulator.o FileSystem.o INode.o OpenFileTable.o SuperBlock.o Utility.o 

all: main test

main: $(OBJS) TestMain

test: $(OBJS) Layer0Test Layer1CombinedTest

Layer1CombinedTest: $(OBJS) Layer1CombinedTest.o
	$(CC) $(CFLAGS) -o $@ $^

TestMain: $(OBJS) TestMain.o
	$(CC) $(CFLAGS) -o $@ $^

Layer0Test: $(OBJS) Layer0Test.o
	$(CC) $(CFLAGS) -o $@ $^
    
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr *.o Layer0Test Layer1CombinedTest TestMain
