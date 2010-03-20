#!/bin/bash

if [ "$1" == '' ];
then
  num=10
else
  num=$1
fi

for (( i=1 ; i<="$num"; i++))
do
  proc=`printf "%02d\n" "$i"`
  echo "ssh particles$((i-1)) cds$proc:~/Documents/homework4"
done
