#!/bin/bash

export OMP_NUM_THREADS=$1
export OMP_SCHEDULE=$2

time make run_parallel
