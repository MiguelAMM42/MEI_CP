
#include "../include/utils.h"

#include "mpi.h"

int main(int argc, char *argv[])
{
    //char *argc1;
    int myID, /*value,*/ size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myID);
    int N, K, T;
    char *p;
    if (argc>=3){
        N = strtol(argv[1], &p, 10);
        K = strtol(argv[2], &p, 10);
        if(argc==4){
            T = strtol(argv[3], &p, 10);
        }else {
            T = -1;
        }
        //N = 10000000;
        //K = 4;
        //T = 1;
        int length_per_process = N / (size);
        int this_pos = (myID)*length_per_process;
        //printf("Olá do processo: %d || %d \n", myID, this_pos);

        initialize(N, K, T);
        kmeans_aux(N, K, T, this_pos, length_per_process);

        MPI_Finalize();
        return 0;
    }else {
        printf("Invalid number of arguments\n");
        return 1;
    }
}