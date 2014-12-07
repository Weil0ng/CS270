#!/bin/bash

prefix="Large"
name=$prefix
Byte=1
Kilo=1024
BLK=4096
Mega=$((1024 * 1024))
size=$(($1 * $Kilo * $Mega))
echo "creating big file $name of size "$size"B"
dd if=/dev/urandom of=$name count=$size bs=$BLK
