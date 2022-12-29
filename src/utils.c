#include "../include/utils.h"

#include "mpi.h"
/*
In this two arrays we store all the samples.
Each array has the x and y component of each sample.
We use align to improve vetorization.
This array's are constant during the execution of the programme.
*/
float *x __attribute__((aligned(32)));
float *y __attribute__((aligned(32)));

// In this array we store the current position occupied in the clusterPos matrix.
int *clusterCurrentPos;

// Says to wich cluster each sample belongs
int *wichCluster;

// In the next two arrays we store the geometric center of each cluster.
// Each array stores the x and y component of each geometric center.
float *geometricCenterX;
float *geometricCenterY;


// 1(a) e (b)
// Here we initialize all samples, and clusters.
void initialize(int N, int K, int T){
    x = malloc(N * sizeof(float));
    y = malloc(N * sizeof(float));

    clusterCurrentPos = malloc(K * sizeof(int));

    wichCluster = malloc(N * sizeof(float));

    geometricCenterX = malloc(K * sizeof(float));
    geometricCenterY = malloc(K * sizeof(float));

    srand(10);
    float inv = (float)1 / (float)RAND_MAX;
    int i;
    for(i=0; i<N; i++){
        x[i] = (float)rand()*inv;
        y[i] = (float)rand()*inv;
    }

    for (int i = 0; i < K; i++)
    {
        // Each cluster has one sample.
        clusterCurrentPos[i] = 0;
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
int attribution(int init, int N, int K, int T)
{
    int argc = 4;
    int change = 0;
    // At the start of each atribution, the algorithm must consider the clusters empty.
   int myID, value, size;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Existem %d coisos \n", size);
    int length_per_process = N/size;
    MPI_Comm_rank(MPI_COMM_WORLD, &myID); 
   printf("AQUI, processo %d diz olá\n", myID) ;
    // Master
    if (myID == 0){
        for (int p=0; p<size-1; p++) {
            printf("Envia para %d\n", p+1);
                    MPI_Send(&length_per_process , 1, MPI_INT, p+1,
                             0, MPI_COMM_WORLD);
                    //MPI_Send(geometricCenterX , K, MPI_FLOAT, p+1, p+1, MPI_COMM_WORLD);
                    //MPI_Send(geometricCenterY , K, MPI_FLOAT, p+1, p+1, MPI_COMM_WORLD);
            /*for (int i=0; i<length_per_process; i++)
                {
                    printf("Envia N");
                } 
                */
        }
    }

    else {
        printf("Filho recebe... %d\n", myID );
        float *geometricCenterXLocal;
        float *geometricCenterYLocal;
        int msg;
        MPI_Recv( &msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
        //MPI_Recv(geometricCenterXLocal, K, MPI_FLOAT, 0, myID, MPI_COMM_WORLD, &status);
        //MPI_Recv(geometricCenterYLocal, K, MPI_FLOAT, 0, myID, MPI_COMM_WORLD, &status);
        printf("Recebi coisas?");
        printf( "Received %d\n", msg);
    }

/*
    for(int i = init; i<N ; i++){
        int bestCluster = -1;
        float clusterMin = (float)RAND_MAX;
        for (int cluster=0; cluster<K; cluster++) {
            
            float distCluster = (x[i] - geometricCenterX[cluster] )*(x[i] - geometricCenterX[cluster]) + (y[i] - geometricCenterY[cluster])*(y[i] - geometricCenterY[cluster]) ;
            
            if (distCluster <= clusterMin) {
                clusterMin = distCluster;
                bestCluster = cluster;
            }
        }
        
        
        float oldCluster = wichCluster[i];

        // Check if the clusters has changed.
        if (oldCluster != bestCluster)
        {
            wichCluster[i] = bestCluster;
            change = 1;
        }
    }
    */
    printf("Finaliza proc: %d\n", myID);
    return change;
}

// Calculate geometric centers.
void geometricCenter(int N, int K, int T)
{

    float sum_clusters_coord_X[K];
    float sum_clusters_coord_Y[K];
    for (int cluster = 0; cluster < K; cluster++)
    {
        sum_clusters_coord_X[cluster] = 0;
        sum_clusters_coord_Y[cluster] = 0;
        clusterCurrentPos[cluster] = -1;
    }
    
    #pragma omp parallel for reduction(+:clusterCurrentPos[:K]) reduction(+:sum_clusters_coord_X) reduction(+:sum_clusters_coord_Y) num_threads(T)
    for(int i = 0; i < N; i++) {
        int currentCluster = wichCluster[i];
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
void kmeans(int N, int K, int T)
{
    // 0 already happen in main function
    int iterationNumber = 0;

    // 3,4
    int change = 1;// TRUE: 1; FALSE: 0 ;
    while(iterationNumber < 21 && change) {
    //while(change == 1) {
    //while(iterationNumber < 40) {
        geometricCenter(N,K,T);
        change  = attribution(0,N,K,T);
        iterationNumber++;
        printf("Fez mais uma iteração\n");
    }

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
    {
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i], geometricCenterY[i], clusterCurrentPos[i] + 1);
    }
    printf("Iterations : %d\n", iterationNumber-1);
    free(x);
    free(y);
    free(wichCluster);
    free(clusterCurrentPos);
    free(geometricCenterX);
    free(geometricCenterY);
}

// Made with love ❤
