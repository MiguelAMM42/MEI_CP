nValue = 1000000
tValues = [1, 2, 4, 8, 12, 16, 20, 24 , 28, 32, 36, 40]
pValues = [10,20]

# create Dir if not exists
import os
if not os.path.exists("Graf2Tests"):
    os.makedirs("Graf2Tests")


for t in tValues:
    for p in pValues:
        f = open("Graf2Tests/testGraf2_" + str(p) + "P_" + str(t) + "T.sh", "w")
        f.write("#!/bin/bash" + "\n" + "#SBATCH --time=1:00" + "\n" + "#SBATCH --ntasks=" + str(p) + "\n" + "#SBATCH --partition=cpar" + "\n" + "module load gcc/7.2.0" + "\n" + "export CP_CLUSTERS=4" + "\n" + "export N_SIZE=" + str(nValue) + "\n" + "export PROCESSES=" + str(p) + "\n" + "export THREADS=" + str(t) + "\n" + "make clean" + "\n" + "make k_means" + "\n" + "perf stat -r 1 -e L1-dcache-load-misses -M cpi make runpar" + "\n"
        + "echo \"---------------------------------------------------------------------\"" + "\n" + "echo \"--- TEST WITH || THREADS: " + str(t) + " || PROCESSES: " +str(p) + "----\"")


