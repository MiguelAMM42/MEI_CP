
#include "../include/utils.h"

int main() {
   
    initialize();
    //testIniatialize();  
    // começar em K pq o K-1 já estão no cluster
    attribution(K);
    kmeans();
    return 0;
}