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

/*
In the next matrix, we associate each sample to one cluster.
Each line corresponds to one cluster. Each position in line stores the position of the sample in the x and y array.
*/
int clusterPos[K][N];

// In this array we store the current position occupied in the clusterPos matrix.
int clusterCurrentPos[K];


// In the next two arrays we store the geometric center of each cluster. 
// Each array stores the x and y component of each geometric center.
float geometricCenterX[K];
float geometricCenterY[K];


// 1(a) e (b)
// Here we initialize all samples, and clusters.
void initialize(){
    srand(10);
    float inv =(float)1 / (float)RAND_MAX;
    int i;
    for(i=0; i<N; i++){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
    }

    for(int i=0; i<K; i++){
        // Each cluster has one sample.
        clusterPos[i][0] = i;
        clusterCurrentPos[i] = 0;
        // The geometric center of the cluster in the initialization corresponds to the only sample they got.
        geometricCenterX[i] = x[i];
        geometricCenterY[i] = y[i];
    }

    
}

// Equal to the previous version, but with loop unrolling.
void initialize2(){
    srand(10);
    float inv =(float)1 / (float)RAND_MAX;
    int i;
    for(i=0; i<N-4; i+=4){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
 
        x[i+1] = (float)rand()*inv;
        y[i+1] = (float)rand()*inv;
        x[i+2] = (float)rand()*inv;
        y[i+2] = (float)rand()*inv;
        x[i+3] = (float)rand()*inv;
        y[i+3] = (float)rand()*inv;
    }
    while(i<N)
    {
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
        i++;
    }
    for(int i=0; i<K; i++){
        clusterPos[i][0] = i;
        clusterCurrentPos[i] = 0;
        geometricCenterX[i] = x[i];
        geometricCenterY[i] = y[i];
    }

    
}

// Function to print the information of the initialization of the program.
void testIniatialize(){
    printf("First 4 positions of matrix:\n");
    for(int i=0; i<4; i++){ //K: 4
        printf("x: %f || y: %f\n", x[i], y[i]);
    }

    printf("\nCluster:\n");
    for(int i=0; i<K; i++){
        printf("x: %f || y: %f\n", x[clusterPos[i][0]], y[clusterPos[i][0]]);
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
    // No início de cada atribuição, temos de fingir que os clusters estão vazios.
    for (int i = init; i < K; i++){
        clusterCurrentPos[i] = 0;
    }
    int i = init;
    for(; i<N && !change; i++){
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
        // Previous positions of the clusters.
        //                           current cluster|position that we want                                        
        float oldXPos = x[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];
        float oldYPos = y[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];

        clusterPos[bestCluster][clusterCurrentPos[bestCluster]] = i;
        clusterCurrentPos[bestCluster]++;
        // Check if the clusters has changed.
        if(oldXPos != x[i] || oldYPos != y[i]){
            change = 1;
        }
    }
    for(; i<N; i++){
        float clusterMin = (float)RAND_MAX;
        int bestCluster = -1;
        for (int cluster=0; cluster<K; cluster++) {
            //float distCluster = sqrt((x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster])*1.0); 
            //float distCluster = sqrt(pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2)*1.0); 
            float distCluster = (x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster]); 
            //float distCluster = pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2); 
            if (distCluster <= clusterMin) {
                clusterMin = distCluster;
                bestCluster = cluster;
            }
        }
        // attribution
        clusterPos[bestCluster][clusterCurrentPos[bestCluster]] = i;
        clusterCurrentPos[bestCluster]++;
    }
    return change;
}

// Calculate geometric centers.
void geometricCenter(){
    float tmpX;
    float tmpY;
    for(int cluster = 0; cluster < K; cluster++) {
        tmpX = 0;
        tmpY = 0;
        int j = 0;
        for(; j <= clusterCurrentPos[cluster]; j++){
            tmpX += x[clusterPos[cluster][j]];
            tmpY += y[clusterPos[cluster][j]];
        }
        
        geometricCenterX[cluster] = tmpX/(clusterCurrentPos[cluster]+1);
        geometricCenterY[cluster] = tmpY/(clusterCurrentPos[cluster]+1);
    }
}

// 2
// Equal to the previous version, but with loop unrolling.
void geometricCenter2(){
    for(int cluster = 0; cluster < K; cluster++) {
        float tmpX = 0;
        float tmpY = 0;
        int j = 0;
        for(; j <= clusterCurrentPos[cluster]-2; j+=2){
            tmpX += x[clusterPos[cluster][j]];
            tmpY += y[clusterPos[cluster][j]];
            tmpX += x[clusterPos[cluster][j+1]];
            tmpY += y[clusterPos[cluster][j+1]];
            
        }
        while (j <= clusterCurrentPos[cluster]){
            tmpX += x[clusterPos[cluster][j]];
            tmpY += y[clusterPos[cluster][j]];
            j++;
        }
        geometricCenterX[cluster] = tmpX/(clusterCurrentPos[cluster]+1);
        geometricCenterY[cluster] = tmpY/(clusterCurrentPos[cluster]+1);
    }
}


// 3,4

void kmeans(){
    // 0 já aconteceu
    int iterationNumber = 1;

    // 3,4
    int change = 1;// TRUE: 1; FALSE: 0 ;
    while(change!=0) {
        geometricCenter2();
        change  = attribution(0);
        iterationNumber++;
    }

    printf("N = %d, K = %d\n",N,K);
    for(int i = 0; i < K; i++){
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i],geometricCenterY[i],clusterCurrentPos[i]);
    }
    printf("Iterations : %d\n", iterationNumber);

}


