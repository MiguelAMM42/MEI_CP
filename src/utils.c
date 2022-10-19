#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/utils.h"


float x[N];
float y[N];

float clusterX[K][N];
float clusterY[K][N];

int clusterLastPos[K];


// depois pode ficar um array e acede-se a 2 de cada vez
float geometricCenterX[K];
float geometricCenterY[K];


// 1(a) e (b)
void initialize(){
    srand(10);
    for(int i=0; i<N; i++){
        x[i] = (float)rand()/(float)RAND_MAX;
        y[i] = (float)rand()/(float)RAND_MAX;
    }
    for(int i=0; i<K; i++){
        clusterX[i][0] = x[i];
        clusterY[i][0] = y[i];
        clusterLastPos[i] = 0;
        geometricCenterX[i] = x[i];
        geometricCenterY[i] = y[i];
    }
}

void testIniatialize(){
    printf("First 4 positions of matrix:\n");
    for(int i=0; i<4; i++){
        printf("x: %f || y: %f\n", x[i], y[i]);
    }

    printf("\nCluster:\n");
    for(int i=0; i<2; i++){
        printf("x: %f || y: %f\n", clusterX[i][0], clusterY[i][0]);
    }
}

// 1(c)

void attribution(int init){
    
    for(int i=init; i<N; i++){
        double clusterMin = (double)RAND_MAX;
        int bestCluster = -1;
        for (int cluster=0; i<K; i++) {
            printf("gcX : %f\n", geometricCenterX[i]);
            printf("gcY : %f\n", geometricCenterY[i]);
            double distCluster = sqrt(pow(geometricCenterX[i] - x[i], 2) + pow(geometricCenterY[i] - y[i], 2) * 1.0); 
            printf("distCluster: %f\n",distCluster);
            printf("clusterMIn: %f\n",clusterMin);
            printf("bestCluster: %d\n\n", bestCluster);
            if (distCluster < clusterMin) {
                clusterMin = distCluster;
                bestCluster = cluster;
            }
        }
        // attribution
        clusterX[bestCluster][clusterLastPos[bestCluster]+1] = x[i];
        clusterY[bestCluster][clusterLastPos[bestCluster]+1] = y[i];
        clusterLastPos[bestCluster]++;
    }
}


// 2
void geometricCenter(){
    float tmpX;
    float tmpY;
    for(int cluster = 0; cluster < K; cluster++) {
        tmpX = 0;
        tmpY = 0;
        for(int j = 0 ; j <= clusterLastPos[cluster]; j++){
            tmpX += clusterX[cluster][j];
            tmpY += clusterY[cluster][j];
        }
        geometricCenterX[cluster] = tmpX/(clusterLastPos[cluster]+1);
        geometricCenterY[cluster] = tmpY/(clusterLastPos[cluster]+1);
    }
}


// 3,4

void kmeans(){

    int numIterations = 0;

    // 2
    geometricCenter();

    // 3,4
    while(numIterations<10) {

        printf("Number of iterations: %d\n", numIterations);

        attribution(0);

        numIterations++;
    }

    printf("\nkmeans\n");

    printf("Number of iterations: %d\n", numIterations);

}


float distance(int x1, int y1, int x2, int y2)
{
    // Calculating distance
    return sqrt(pow(x2 - x1, 2)
                + pow(y2 - y1, 2) * 1.0);
}
