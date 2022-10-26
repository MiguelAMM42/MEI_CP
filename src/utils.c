#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/utils.h"


float x[N]  __attribute__ ((aligned (32)));
float y[N]  __attribute__ ((aligned (32)));

int clusterPos[K][N];


int clusterCurrentPos[K];


// depois pode ficar um array e acede-se a 2 de cada vez
float geometricCenterX[K];
float geometricCenterY[K];


// 1(a) e (b)
void initialize(){
    srand(10);
    float inv =(float)1 / (float)RAND_MAX;
    int i;
    for(i=0; i<N; i++){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
    }

    for(int i=0; i<K; i++){
        clusterPos[i][0] = i;
        clusterCurrentPos[i] = 0;
        geometricCenterX[i] = x[i];
        geometricCenterY[i] = y[i];
    }

    
}


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
            //float distCluster = sqrt(pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2)*1.0); 
            //float distCluster = sqrt((x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster])*1.0); 
            float distCluster = (x[i] - geometricCenterX[cluster])*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster]); 
            //float distCluster = pow(geometricCenterX[cluster] - x[i], 2) + pow(geometricCenterY[cluster] - y[i], 2); 
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


// 2
// Versão com loop
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
// Versão sem loop unrolling
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


