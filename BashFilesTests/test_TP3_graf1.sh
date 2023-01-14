#!/bin/bash


module load gcc/7.2.0 
export CP_CLUSTERS=4
for n in 1000 10000 100000 1000000 10000000
do
    for p in 1 2 4 8 16 20 32 40 64 128
    do 
        echo ""
        echo "---------------------------------------------------------------------"
        echo "--- TEST WITH || SIZE: $n || PROCESSES: $p || ---"
        #SBATCH --time=1:00
        #SBATCH --ntasks=$p
        #SBATCH --partition=cpar
        export N_SIZE=$n
        export PROCESSES=$p
        make clean
        make k_means
        perf stat -r 1 -e L1-dcache-load-misses -M cpi make runseq
        echo "---------------------------------------------------------------------"
        echo ""
    done
done
