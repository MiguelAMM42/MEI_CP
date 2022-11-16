
#include "../include/utils.h"


int main(int argc,char* argv[]) {
    int N,K,T;
    char* p;
    if (argc>=3){
        N = strtol(argv[1], &p, 10);
        K = strtol(argv[2], &p, 10);
        if(argc==4){
            T = strtol(argv[3], &p, 10);
        }else {
            T = -1;
        }
        initialize(N,K,T);
        //testIniatialize();  
        attribution(K,N,K,T);
        kmeans(N,K,T);
        return 0;
    }else {
        printf("Invalid number of arguments\n");
        return 1;
    }
}