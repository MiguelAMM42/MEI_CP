nValues = [1000, 10000, 100000, 1000000, 10000000]
pValues = [1, 2, 4, 8, 16, 20, 32, 40, 64, 128]

# create Dir if not exists
import os
if not os.path.exists("Graf1Tests"):
    os.makedirs("Graf1Tests")


for n in nValues:
    for p in pValues:
        f = open("Graf1Tests/testGraf1_" + str(n) + "_" + str(p) + ".sh", "w")
        f.write("#!/bin/bash" + "\n" + "#SBATCH --time=1:00" + "\n" + "#SBATCH --ntasks=" + str(p) + "\n" + "#SBATCH --partition=cpar" + "\n" + "module load gcc/7.2.0" + "\n" + "export CP_CLUSTERS=4" + "\n" + "export N_SIZE=" + str(n) + "\n" + "export PROCESSES=" + str(p) + "\n" + "make clean" + "\n" + "make k_means" + "\n" + "perf stat -r 1 -e L1-dcache-load-misses -M cpi make runseq" + "\n"
        + "echo \"---------------------------------------------------------------------\"" + "\n" + "echo \"--- TEST WITH || SIZE: " + str(n) + " || PROCESSES: " +str(p) + "----\"")
