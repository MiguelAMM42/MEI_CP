CC = gcc
BIN = bin/
SRC = src/
INCLUDES = include/
EXEC = k_means
CP_CLUSTERS = 4
THREADS = 2

CFLAGSBASE = -O3  -fopenmp -Wall -Wextra 
CFLAGSLOOP = -O3 -fopenmp -Wall -Wextra -funroll-loops
CFLAGSVEC = -O3 -fopenmp  -Wall -Wextra -ftree-vectorize -msse4
ASSEMBLY = -S -o
ASSEMBLYLOOP = -O2 -g -Wall -Wextra -funroll-loops -S -o
ASSEMBLYVEC = -O2 -g -Wall -Wextra -ftree-vectorize -msse4 -S -o
ASSEMBLYTH = -O3 -fopenmp -Wall -Wextra -funroll-loops -S -o
LIBS = -lm

.DEFAULT_GOAL = k_means

base_noflags: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

base: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSBASE) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

$(BIN)utils.o: $(SRC)utils.c $(INCLUDES)utils.h
	$(CC) $(CFLAGSLOOP) -c $(SRC)utils.c -o $(BIN)utils.o

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

k_means: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSLOOP) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

clean:
	rm -r bin/*

run:
	./$(BIN)$(EXEC)

runseq:
	./$(BIN)$(EXEC) 10000000 $(CP_CLUSTERS) 

runpar:
	./$(BIN)$(EXEC) 10000000 $(CP_CLUSTERS) $(THREADS)
