CC = gcc
BIN = bin/
SRC = src/
INCLUDES = include/
EXEC = k_means

CFLAGS = -O2 -g -Wall -Wextra 
CFLAGSASS = -O2 -g -Wall -Wextra -funroll-loops
ASSEMBLY = -S -o
LIBS = -lm

.DEFAULT_GOAL = k_means

k_means: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGS) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)

$(BIN)utils.o: $(SRC)utils.c $(INCLUDES)utils.h
	$(CC) $(CFLAGS) -c $(SRC)utils.c -o $(BIN)utils.o

loop: $(SRC)k_means.c $(BIN)utils.o
	$(CC) $(CFLAGSASS) $(SRC)k_means.c $(BIN)utils.o -o $(BIN)$(EXEC) $(LIBS)
assembly:
	$(CC) $(ASSEMBLY) $(BIN)k_means_assembly.s $(SRC)utils.c

clean:
	rm -r bin/*

run:
	./$(BIN)$(EXEC)