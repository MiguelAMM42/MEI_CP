#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/utils.h"

/*
In this two arrays we store all the samples.
Each array has the x and y component of each sample.
We use align to improve vetorization.
This array's are constant during the execution of the programme.
*/
float x[N]  __attribute__ ((aligned (32)));
float y[N]  __attribute__ ((aligned (32)));

// In this array we store the current position occupied in the clusterPos matrix.
int clusterCurrentPos[K];

// Says to wich cluster each sample belongs
int wichCluster[N];


// In the next two arrays we store the geometric center of each cluster. 
// Each array stores the x and y component of each geometric center.
float geometricCenterX[K];
float geometricCenterY[K];


// 1(a) e (b)
// Here we initialize all samples, and clusters.
void initialize(){
    printf("Wich\n");
    srand(10);
    float inv =(float)1 / (float)RAND_MAX;
    int i;
    for(i=0; i<N; i++){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
        // Se calhar pode-se remover a linha de baixo
        //int xx = 10000000 -50;
        //if (i >xx ) printf("X: %f| Y: %f\n", x[i], y[i]);
        //wichCluster[i] = -1;
    }

    for(int i=0; i<K; i++){
        // Each cluster has one sample.
        wichCluster[i] = i;
        // The geometric center of the cluster in the initialization corresponds to the only sample they got.
        geometricCenterX[i] = x[i];
        geometricCenterY[i] = y[i];
    }

    
}






/*
Here we atribute the samples to the clusters.
We keep other ways of checking distances so we could test in the future other versions of our program.
Also, we have two cycles because we check here if the clusters has changes to the previous versions. 
So, to avoid checking N*K times if the cluster has changed, we have the change flag. When we find changes, we stop checking. 
*/
int attribution(int init){
    int change = 0;
    // At the start of each atribution, the algorithm must consider the clusters empty.
    int i = init;
    #pragma omp parallel
    for(; i<N ; i++){
        float clusterMin = (float)RAND_MAX;
        int bestCluster = -1;
        for (int cluster=0; cluster<K; cluster++) {
            //float distCluster = sqrt(pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2)*1.0); 
            //float distCluster = sqrt((x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster])*1.0); 
            float distCluster = (x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster]); 
            //float distCluster = pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2); 
            if (distCluster <= clusterMin) {
                clusterMin = distCluster;
                bestCluster = cluster;
            }
        }
        //if (i < 50 ) 
        //printf("Best cluster is: %d | %f\n", bestCluster, clusterMin);
        // Previous positions of the clusters.
        //                           current cluster|position that we want                                        
        //float oldXPos = x[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];
        //float oldYPos = y[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];

        float oldCluster = wichCluster[i];
        wichCluster[i] = bestCluster;
        // Check if the clusters has changed.
        if(oldCluster != bestCluster){
            change = 1;
        }
    }
   return change;
}

// Calculate geometric centers.
void geometricCenter(){

    float sum_clusters_coord_X[K];
    float sum_clusters_coord_Y[K];
    for(int cluster = 0; cluster < K; cluster++) {
        sum_clusters_coord_X[cluster] = 0;
        sum_clusters_coord_Y[cluster] = 0;
        clusterCurrentPos[cluster] = -1;
    }

    for(int i = 0; i < N; i++) {
        int currentCluster = wichCluster[i];
        //if (i < 30)
        //printf("Cluster atual: %d\n", currentCluster);
        sum_clusters_coord_X[currentCluster] += x[i];
        sum_clusters_coord_Y[currentCluster] += y[i];
        clusterCurrentPos[currentCluster] += 1;
    }
    for(int cluster = 0; cluster < K; cluster++) {
        //printf("Sum clusters: %f | %f\n ", sum_clusters_coord_X[cluster], sum_clusters_coord_Y[cluster]);
        //printf("How mant clusters: %d \n ", clusterCurrentPos[cluster]);
        geometricCenterX[cluster] = sum_clusters_coord_X[cluster]/(clusterCurrentPos[cluster]+1);
        geometricCenterY[cluster] = sum_clusters_coord_Y[cluster]/(clusterCurrentPos[cluster]+1);
    }
}

// 3,4
// Main function
void kmeans(){
    // 0 already happen in main function
    int iterationNumber = 0;

    // 3,4
    int change = 1;// TRUE: 1; FALSE: 0 ;
    while(iterationNumber < 21) {
    //while(change == 1) {
    //while(iterationNumber < 40) {
        geometricCenter();
        printf("Iteracion number %d \n", iterationNumber);
       // for(int i = 0; i < K; i++){
       //     printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i],geometricCenterY[i],clusterCurrentPos[i]);
       // }
        change  = attribution(0);
        iterationNumber++;
    }

    printf("N = %d, K = %d\n",N,K);
    for(int i = 0; i < K; i++){
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i],geometricCenterY[i],clusterCurrentPos[i]+1);
    }
    printf("Iterations : %d\n", iterationNumber);

}


