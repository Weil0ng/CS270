#!/bin/bash

number=$1
prefix="dir"
i=1
while [ $i -le $number ]
  do
    name=$prefix$i
    echo "mkdir $name"
    mkdir $name
    i=$((i + 1))
  done
