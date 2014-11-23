#This is the makefile to make the emulator
CC=gcc
LD=gcc

CFLAGS=-O2 -std=gnu99 -g
OBJS=Directories.o DiskEmulator.o FileSystem.o INode.o SuperBlock.o Utility.o OpenFileTable.o
FUSEFLAGS=`pkg-config fuse --cflags --libs`
SRCS=fuseDaemon.c

all: fuse main test init

init: $(OBJS) InitFS

fuse: $(OBJS) fuseDaemon

main: $(OBJS) TestMain

test: $(OBJS) Layer0Test Layer1CombinedTest Layer2MountTest

InitFS: $(OBJS) InitFS.o
	$(CC) $(CFLAGS) -o $@ $^

fuseDaemon: $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(FUSEFLAGS) -o $@ $(OBJS)

TestMain: $(OBJS) TestMain.o
	$(CC) $(CFLAGS) -o $@ $^

Layer0Test: $(OBJS) Layer0Test.o
	$(CC) $(CFLAGS) -o $@ $^

Layer1CombinedTest: $(OBJS) Layer1CombinedTest.o
	$(CC) $(CFLAGS) -o $@ $^

Layer2MountTest: $(OBJS) Layer2MountTest.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr *.o Layer0Test Layer1CombinedTest Layer2MountTest TestMain fuseDaemon diskFile diskDump

