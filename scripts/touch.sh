#!/bin/bash

number=$1
prefix="nod"
i=1
while [ $i -le $number ]
  do
    name=$prefix$i
    echo "touch $name"
    touch $name
    i=$((i + 1))
  done
