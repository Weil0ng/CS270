#!/bin/bash

size=$1
prefix="Big"
name=$prefix
echo "creating big file $name of size "$size"G"
truncate -s "$size"G $name
