#!/bin/bash

# a simple bash script for running repeated trials
# because time outputs results on stderr, 2>&1 redirects
# stderr to stdout so that the timing results can be
# piped to a file

for (( thread_count=500; thread_count<=4500; thread_count=thread_count+1000 ))
do
    /usr/bin/time 2>&1 --format "%e" ./pmandel2 8 4 $thread_count 200000 0
done
