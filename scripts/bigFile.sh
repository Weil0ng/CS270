#!/bin/bash

prefix="Large"
name=$prefix
Kilo=1024
Mega=$((1024 * 1024))
size=$(($1 * $Mega))
echo "creating big file $name of size "$size"G"
dd if=/dev/urandom of=$name count=$size bs=$Kilo
