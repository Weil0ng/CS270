#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-O2 -std=gnu99 -g
FUSEFLAGS=`pkg-config fuse --cflags --libs`
OBJS=Directories.o DiskEmulator.o FileSystem.o INode.o SuperBlock.o Utility.o OpenFileTable.o
SRCS=fuseDaemon.c

all: main test fuseDaemon

main: $(OBJS) TestMain

test: $(OBJS) Layer0Test Layer1CombinedTest

fuseDaemon: $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(FUSEFLAGS) -o $@ $(OBJS)

Layer1CombinedTest: $(OBJS) Layer1CombinedTest.o
	$(CC) $(CFLAGS) -o $@ $^

TestMain: $(OBJS) TestMain.o
	$(CC) $(CFLAGS) -o $@ $^

Layer0Test: $(OBJS) Layer0Test.o
	$(CC) $(CFLAGS) -o $@ $^
    
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr *.o Layer0Test Layer1CombinedTest TestMain fuseDaemon diskFile diskDump
