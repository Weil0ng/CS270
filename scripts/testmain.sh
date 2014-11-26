#!/bin/bash

usage() {
  echo "testmain -c [cmd]"
}

numDir=100
numFile=100
unset opt

getopts ":c:" opt
if [ "$opt" = "?" ]
then
  usage
  exit
fi

case $OPTARG in
  mkdir)
    echo "test mkdir"
    sh ./mkdir.sh $numDir
    ;;
  touch)
    echo "test touch"
    sh ./touch.sh $numFile
esac

