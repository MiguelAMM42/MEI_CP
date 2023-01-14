#!/bin/bash
#SBATCH --time=1:00
#SBATCH --ntasks=8
#SBATCH --partition=cpar
module load gcc/7.2.0
export CP_CLUSTERS=4
export N_SIZE=10000000
export PROCESSES=8
make clean
make k_means
perf stat -r 1 -e L1-dcache-load-misses -M cpi make runseq
echo "---------------------------------------------------------------------"
echo "--- TEST WITH || SIZE: 10000000 || PROCESSES: 8----"