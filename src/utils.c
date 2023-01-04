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
int *wichCluster_temp;

// In the next two arrays we store the geometric center of each cluster.
// Each array stores the x and y component of each geometric center.
float *geometricCenterX;
float *geometricCenterY;

MPI_Status status;
// 1(a) e (b)
// Here we initialize all samples, and clusters.
void initialize(int N, int K, int T)
{
    x = malloc(N * sizeof(float));
    y = malloc(N * sizeof(float));

    clusterCurrentPos = malloc(K * sizeof(int));

    wichCluster = malloc(N * sizeof(int));
    wichCluster_temp = malloc(N * sizeof(int));

    geometricCenterX = malloc(K * sizeof(float));
    geometricCenterY = malloc(K * sizeof(float));

    srand(10);
    float inv = (float)1 / (float)RAND_MAX;
    int i;
    for (i = 0; i < N; i++)
    {
        x[i] = (float)rand() * inv;
        y[i] = (float)rand() * inv;
        // Provavelmente depois vai ser para tirar
        wichCluster[i] = -1;
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

// Calculate geometric centers.
// Depois meter openMP dentro do MPI
void geometricCenter(int N, int K, int T, int pos_start, int length_per_process)
{

    float sum_clusters_coord_X[K];
    float sum_clusters_coord_Y[K];
    for (int cluster = 0; cluster < K; cluster++)
    {
        sum_clusters_coord_X[cluster] = 0;
        sum_clusters_coord_Y[cluster] = 0;
        clusterCurrentPos[cluster] = -1;
    }

    for (int i = 0; i < length_per_process; i++)
    {
        int pos_atual = i + pos_start;
        if (pos_atual >= N)
        {
            break;
        }
        int currentCluster = wichCluster[pos_atual];
        sum_clusters_coord_X[currentCluster] += x[pos_atual];
        sum_clusters_coord_Y[currentCluster] += y[pos_atual];
        clusterCurrentPos[currentCluster] += 1;
    }
    // Reduces com todos
    // Todos têm as somas todas
    // Todos fazem as divisões

    float sum_clusters_coord_X_final[K];
    float sum_clusters_coord_Y_final[K];
    int clusterCurrentPos_final[K];
    for (int i = 0; i < K; i++)
    {

        MPI_Allreduce(&(sum_clusters_coord_X[i]), &(sum_clusters_coord_X_final[i]), 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&(sum_clusters_coord_Y[i]), &(sum_clusters_coord_Y_final[i]), 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&(clusterCurrentPos[i]), &(clusterCurrentPos_final[i]), 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }
    for (int cluster = 0; cluster < K; cluster++)
    {
        // printf("Sum clusters: %f | %f\n ", sum_clusters_coord_X[cluster], sum_clusters_coord_Y[cluster]);
        // printf("How mant clusters: %d \n ", clusterCurrentPos[cluster]);
        geometricCenterX[cluster] = sum_clusters_coord_X_final[cluster] / (clusterCurrentPos_final[cluster] + 1);
        geometricCenterY[cluster] = sum_clusters_coord_Y_final[cluster] / (clusterCurrentPos_final[cluster] + 1);
    }
}

int attribution_aux(int N, int K, int pos_start, int length_per_process)
{
    int change = 0;
    // Este depois pode ser removido e passado como argumento
    // O myID também é desnecessário
    // Envia posição a partir da qual o outro deve analisar

    // Analisa o array do X e do Y, e descobre qual é o melhor cluster.
    for (int i = 0; i < length_per_process; i++)
    {
        int bestCluster = -1;
        int pos_atual = i + pos_start;
        if (pos_atual >= N)
        {
            break;
        }
        float clusterMin = (float)RAND_MAX;
        for (int cluster = 0; cluster < K; cluster++)
        {
            if (i == length_per_process - 1)
            {
                printf("\n\nProcesso: %d\n", pos_start);
                printf("Centro geométrico X: %f\n", geometricCenterX[cluster]);
                printf("Centro geométrico Y: %f\n", geometricCenterY[cluster]);
            }
            float distCluster = (x[pos_atual] - geometricCenterX[cluster]) * (x[pos_atual] - geometricCenterX[cluster]) + (y[pos_atual] - geometricCenterY[cluster]) * (y[pos_atual] - geometricCenterY[cluster]);

            if (distCluster <= clusterMin)
            {
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

    // Envia qual a posição atual para o master saber onde deve começar a guardar
    // MPI_Send(&pos_start, 1, MPI_INT, 0,
    //         0, MPI_COMM_WORLD);
    // Envia o valor de change
    int global_change;
    // printf("Change que vai enviar: %d\n", change);
    MPI_Allreduce(&change, &global_change, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    // Envia which Cluster
    // MPI_Send(wichCluster, i, MPI_INT, 0, 0, MPI_COMM_WORLD);

    MPI_Allgather(&(wichCluster[pos_start]), length_per_process, MPI_INT, wichCluster_temp, length_per_process, MPI_INT, MPI_COMM_WORLD);
    wichCluster = wichCluster_temp;

    return global_change;
}

// Falta dar free aqui, nos outros processor
void kmeans_aux(int N, int K, int T, int pos_atual, int length_per_process)
{
    attribution_aux(N, K, pos_atual, length_per_process);
    int iterationNumber = 0;
    int continueCycle = 1;
    while (continueCycle > 0 && iterationNumber < 40)
    {
        geometricCenter(N, K, T, pos_atual, length_per_process);
        iterationNumber++;
        if (pos_atual == 0)
        {
            printf("Fez mais uma iteração %d\n", iterationNumber);
        }
        continueCycle = attribution_aux(N, K, pos_atual, length_per_process);
    }
    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
    {
        printf("Processo: %d\n", pos_atual);
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i], geometricCenterY[i], clusterCurrentPos[i] + 1);
    }
    printf("Iterations : %d\n", iterationNumber - 1);
    printf("Bye\n");
    // Falta dar frees
    /*
    printf("Fim 1\n");
        free(x);
        printf("Fim 2\n");
        free(y);
        printf("Fim 3\n");
        // free(wichCluster);
        printf("Fim 4\n");
        free(clusterCurrentPos);
        printf("Fim 5\n");
        free(geometricCenterX);
        printf("Fim 6\n");
        free(geometricCenterY);
        printf("Fim 7\n");
    */
}

// Made with love ❤
