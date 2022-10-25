#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/utils.h"


float x[N];
float y[N];

int clusterPos[K][N];


int clusterCurrentPos[K];


// depois pode ficar um array e acede-se a 2 de cada vez
float geometricCenterX[K];
float geometricCenterY[K];


// 1(a) e (b)
void initialize(){
    srand(10);
    float inv =(float)1 / (float)RAND_MAX;
    // Depois mudar isto!!! Multiplicar pelo inverso
    for(int i=0; i<N; i++){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
    }
    for(int i=0; i<K; i++){
        clusterPos[i][0] = i;
        clusterCurrentPos[i] = 0;
        //geometricCenterX[i] = x[i];
        //geometricCenterY[i] = y[i];
    }
}

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
           // float distCluster = sqrt(pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2)*1.0); 
            float distCluster = sqrt( pow(x[i] -geometricCenterX[cluster], 2) + pow(y[i] - geometricCenterY[cluster], 2)); 
            if (distCluster <= clusterMin) {
                clusterMin = distCluster;
                bestCluster = cluster;
            }
        }
        // attribution
        float oldXPos = x[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];
        float oldYPos = y[clusterPos[bestCluster][clusterCurrentPos[bestCluster]]];

        clusterPos[bestCluster][clusterCurrentPos[bestCluster]] = i;
        clusterCurrentPos[bestCluster]++;
        if(oldXPos != x[i] || oldYPos != y[i]){
            change = 1;
        }
    }
    for(; i<N; i++){
        float clusterMin = (float)RAND_MAX;
        int bestCluster = -1;
        for (int cluster=0; cluster<K; cluster++) {
           // float distCluster = sqrt(pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2)*1.0); 
            float distCluster = sqrt( pow(x[i] -geometricCenterX[cluster], 2) + pow(y[i] - geometricCenterY[cluster], 2)); 
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


// 2
void geometricCenter(){
    float tmpX;
    float tmpY;
    for(int cluster = 0; cluster < K; cluster++) {
        tmpX = 0;
        tmpY = 0;
        for(int j = 0 ; j <= clusterCurrentPos[cluster]; j++){
            tmpX += x[clusterPos[cluster][j]];
            tmpY += y[clusterPos[cluster][j]];
        }
        geometricCenterX[cluster] = tmpX/(clusterCurrentPos[cluster]+1);
        geometricCenterY[cluster] = tmpY/(clusterCurrentPos[cluster]+1);
    }
}


// 3,4

void kmeans(){
    //attribution(K);
    // 0 já aconteceu
    int iterationNumber = 1;

    // 2
    //geometricCenter();

    // 3,4
    int change = 1;// TRUE: 1; FALSE: 0 ; 2 is easier for shift than 1  
    while(change!=0) {
    
    //while(numberOfIterations < 38) {

        //printf("Iteraction number : %d\n", iterationNumber);
        geometricCenter();
        change  = attribution(0);
        //geometricCenter();
        iterationNumber++;
    }

    //printf("\nkmeans\n");

    //printf("Number of iterations: %d\n", numIterations);
    printf("N = %d, K = %d\n",N,K);
    for(int i = 0; i < K; i++){
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i],geometricCenterY[i],clusterCurrentPos[i]);
    }
    printf("Iterations : %d\n", iterationNumber);

}


