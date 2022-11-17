CC = gcc
BIN = bin/
SRC = src/
INCLUDES = include/
EXEC = k_means
EXEC_SEQ = k_means_seq
EXEC_TH = k_means_th
THREADS = 40

CFLAGSBASE = -O3  -fopenmp -Wall -Wextra 
CFLAGSLOOP = -O3 -fopenmp -Wall -Wextra -funroll-loops
CFLAGSVEC = -O3 -fopenmp  -Wall -Wextra -ftree-vectorize -msse4
ASSEMBLY = -S -o
ASSEMBLYLOOP = -O3 -g -Wall -Wextra -funroll-loops -S -o
ASSEMBLYVEC = -O3 -g -Wall -Wextra -ftree-vectorize -msse4 -S -o
ASSEMBLYTH = -O3 -fopenmp -Wall -Wextra -funroll-loops -S -o
LIBS = -lm
SEQFLAGS= -O3 -Wall -Wextra -funroll-loops
THFLAGS= -O3 -fopenmp -Wall -Wextra -funroll-loops

.DEFAULT_GOAL = k_means

base_noflags: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

base: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSBASE) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

$(BIN)utilsSEQ.o: $(SRC)utils.c $(INCLUDES)utils.h
	$(CC) $(SEQFLAGS) -c $(SRC)utils.c -o $(BIN)utilsSEQ.o

$(BIN)utilsTH.o: $(SRC)utils.c $(INCLUDES)utils.h
	$(CC) $(THFLAGS) -c $(SRC)utils.c -o $(BIN)utilsTH.o

loop: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSLOOP) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

vec: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSVEC) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

assembly:
	$(CC) $(ASSEMBLY) $(BIN)k_means_assembly.s $(SRC)utils.c

assembly_loop:
	$(CC) $(ASSEMBLYLOOP) $(BIN)k_means_assembly.s $(SRC)utils.c

assembly_vec:
	$(CC) $(ASSEMBLYVEC) $(BIN)k_means_assembly.s $(SRC)utils.c

assembly_th:
	$(CC) $(ASSEMBLYTH) $(BIN)k_means_assembly_th.s $(SRC)utils.c

k_means: $(SRC)k_means.c $(BIN)utilsSEQ.o $(BIN)utilsTH.o
	$(CC) $(SEQFLAGS) $(SRC)k_means.c $(BIN)utilsSEQ.o -o $(BIN)$(EXEC_SEQ) $(LIBS)
	$(CC) $(THFLAGS) $(SRC)k_means.c $(BIN)utilsTH.o -o $(BIN)$(EXEC_TH) $(LIBS)

clean:
	rm -r bin/*



runseq:
	./$(BIN)$(EXEC_SEQ) 10000000 $(CP_CLUSTERS) 

runpar:
	./$(BIN)$(EXEC_TH) 10000000 $(CP_CLUSTERS) $(THREADS)
