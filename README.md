==== Team Members ====

Weilong Cui
Jia Zhan
Jonathan Sun

==== How to build ====

make test

    Builds the targets for testing Layer 0 and Layer 1.
    Required for Layer0Test and Layer1CombinedTest.

make main

    Builds the target for testing Layer 2.
    
==== How to run ====

./Layer0Test DISK_SIZE

    Tests the Layer 0 disk emulator.  Makes a disk and attempts
    some simple, predetermined writes on it.
    
    DISK_SIZE is in bytes.

./Layer1CombinedTest MODE FS_NUM_DATA_BLOCKS FS_NUM_INODES

    Tests the Layer 1 functionality by making a file system
    and running a number of predetermined stress tests like
    allocating every inode/data block and writing data.
    
    MODE is either "i" or "d", "i" to test the inode functionality,
    "d" to test the data block functionality.
    
    FS_NUM_DATA_BLOCKS and FS_NUM_INODES determine the size of the
    file system, with some minimum values.

./TestMain FS_NUM_DATA_BLOCKS FS_NUM_INODES   
   
    Tests the Layer 2 functionality by making a file system
    with the parameters specified and giving a menu of 
    commands.  Currently, mkdir, mknod, unlink, open, close, read, write
    system calls are implemented and tested.
   
    In addition to these syste calls, "quit" allows you to exit the program;
    "stats" is a debugging function that will show you the state of the FS
    after makefs or any command.
