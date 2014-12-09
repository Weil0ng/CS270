#!/bin/bash

usage() {
  echo "testmain -c [cmd]"
}

numDir=10000
numFile=10000
bigFileSize=15
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
    ;;
  big)
    echo "test bigFile"
    sh ./bigFile.sh $bigFileSize
    ;;
  bonnie)
    echo "test bonnie"
    bonnie++ -s 4096 -r 2048 -n 4 -m TEST -b -u root
    ;;
esac

