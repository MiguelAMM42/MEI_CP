#!/bin/bash
#SBATCH --time=1:00
#SBATCH --ntasks=20
#SBATCH --partition=cpar
module load gcc/7.2.0
export CP_CLUSTERS=4
export N_SIZE=1000000
export PROCESSES=20
export THREADS=4
make clean
make k_means
perf stat -r 1 -e L1-dcache-load-misses -M cpi make runpar
echo "---------------------------------------------------------------------"
echo "--- TEST WITH || THREADS: 4 || PROCESSES: 20----"