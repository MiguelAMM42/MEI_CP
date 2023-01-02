
#include "../include/utils.h"

#include "mpi.h"

int main(int argc, char *argv[])
{
    char *argc1;
    int myID, value, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myID);
    int N, K, T;
    char *p;
    N = 10000000;
    K = 4;
    T = 1;
    int length_per_process = N / (size);
    int this_pos = (myID)*length_per_process;
    printf("Olá do processo: %d || %d \n", myID, this_pos);
    if (myID == 0)
    {
        initialize(N, K, T);
        // testIniatialize();
        attribution(K, N, K, size);
        kmeans(N, K, T, size);
    }
    else
    {
        initialize(N, K, T);
        kmeans_aux(N, K, T, this_pos);
    }

    MPI_Finalize();
    return 0;
}