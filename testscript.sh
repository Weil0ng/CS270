#!/bin/bash

number=15
i=1
while [ [ $i -le $number ] ]
  do
    echo "mkdir $i"
    mkdir $i
    (i = i + 1)
  done
