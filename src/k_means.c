
#include "../include/utils.h"

#include "mpi.h"

int main(int argc,char* argv[]) {
    char* argc1;
    MPI_Init(&argc1, NULL);
    int N,K,T;
    char* p;
        N = 10000;
        K = 4;
        T = 1;
        initialize(N,K,T);
        //testIniatialize();  
        attribution(K,N,K,T);
        kmeans(N,K,T);

    MPI_Finalize();
        return 0;
}