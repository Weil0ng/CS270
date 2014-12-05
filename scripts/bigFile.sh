#!/bin/bash

prefix="Large"
name=$prefix
Byte=1
Kilo=1024
Mega=$((1024 * 1024))
size=$(($1 * $Byte))
echo "creating big file $name of size "$size"B"
dd if=/dev/urandom of=$name count=$size bs=$Byte
