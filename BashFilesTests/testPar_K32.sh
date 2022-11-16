#!/bin/bash


module load gcc/7.2.0 
export CP_CLUSTERS=32
make clean
make k_means
perf stat -r 3 -e L1-dcache-load-misses -M cpi make runpar