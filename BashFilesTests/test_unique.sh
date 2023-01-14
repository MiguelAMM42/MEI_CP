#!/bin/bash
#SBATCH --time=1:00
#SBATCH --ntasks=20
#SBATCH --partition=cpar
export N_SIZE=$n
export PROCESSES=$p
make clean
make k_means
perf stat -r 1 -e L1-dcache-load-misses -M cpi make runseq
