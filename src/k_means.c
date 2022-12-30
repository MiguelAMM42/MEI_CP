
#include "../include/utils.h"

#include "mpi.h"

int main(int argc, char *argv[])
{
    char *argc1;
    int myID, value, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Existem %d coisos \n", size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myID);
    printf("AQUI, processo %d diz olá\n", myID);
    int N, K, T;
    char *p;
    N = 10000000;
    K = 4;
    T = 1;
    if (myID == 0)
    {
        initialize(N, K, T);
        // testIniatialize();
        attribution(K, N, K, size);
        kmeans(N, K, T, size);
    }
    else
    {
        printf("Filho %d\n", myID);
        initialize(N, K, T);
        kmeans_aux(N, K, T, myID);
    }

    MPI_Finalize();
    return 0;
}