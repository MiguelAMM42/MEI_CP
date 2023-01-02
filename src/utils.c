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

MPI_Status status;
// 1(a) e (b)
// Here we initialize all samples, and clusters.
void initialize(int N, int K, int T)
{
    x = malloc(N * sizeof(float));
    y = malloc(N * sizeof(float));

    clusterCurrentPos = malloc(K * sizeof(int));

    wichCluster = malloc(N * sizeof(float));

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
int attribution(int init, int N, int K, int P)
{
    // At the start of each atribution, the algorithm must consider the clusters empty.
    // Master
    int length_per_process = N / (P);
    int change = 0;
    MPI_Bcast(&length_per_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // for (int p = 0; p < P - 1; p++)
    //{
    //  Envia quantidade de números, depois pode-se remover
    //  Envia posição a partir da qual o outro deve analisar
    MPI_Bcast(geometricCenterX, K, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(geometricCenterY, K, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //}
    for (int i = 0; i < length_per_process; i++)
    {
        int bestCluster = -1;
        // Aqui a posição atual é igual ao i, porque estamos no master
        int pos_atual = i;
        float clusterMin = (float)RAND_MAX;
        for (int cluster = 0; cluster < K; cluster++)
        {

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

    for (int p = 1; p < P - 1; p++)
    {
        int pos_store = p * length_per_process;
        int this_change;

        // Recebe posição para começar a guardar
        // MPI_Recv(&pos_store, 1, MPI_INT, p + 1, 0, MPI_COMM_WORLD, &status);
        // Change

        MPI_Recv(&this_change, 1, MPI_INT, p, 0, MPI_COMM_WORLD, &status);
        if (this_change > change)
            change = 1;
        // int *temp = malloc(length_per_process * sizeof(int));
        // MPI_Recv(&(wichCluster[pos_store]), length_per_process, MPI_INT, p, 0, MPI_COMM_WORLD, &status);
    }

    MPI_Gather(&(wichCluster[0]), length_per_process, MPI_INT, wichCluster, length_per_process, MPI_INT, 0, MPI_COMM_WORLD);
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

#pragma omp parallel for reduction(+                                                                      \
                                   : clusterCurrentPos[:K]) reduction(+                                   \
                                                                      : sum_clusters_coord_X) reduction(+ \
                                                                                                        : sum_clusters_coord_Y) num_threads(T)
    for (int i = 0; i < N; i++)
    {
        int currentCluster = wichCluster[i];
        sum_clusters_coord_X[currentCluster] += x[i];
        sum_clusters_coord_Y[currentCluster] += y[i];
        clusterCurrentPos[currentCluster] += 1;
    }
    for (int cluster = 0; cluster < K; cluster++)
    {
        // printf("Sum clusters: %f | %f\n ", sum_clusters_coord_X[cluster], sum_clusters_coord_Y[cluster]);
        // printf("How mant clusters: %d \n ", clusterCurrentPos[cluster]);
        geometricCenterX[cluster] = sum_clusters_coord_X[cluster] / (clusterCurrentPos[cluster] + 1);
        geometricCenterY[cluster] = sum_clusters_coord_Y[cluster] / (clusterCurrentPos[cluster] + 1);
    }
}

int number_iteracions = 50;

// 3,4
// Main function
void kmeans(int N, int K, int T, int P)
{
    // 0 already happen in main function
    int iterationNumber = 0;

    // 3,4
    int change = 1; // TRUE: 1; FALSE: 0 ;
    while (iterationNumber < number_iteracions && change)
    {
        // while(change == 1) {
        // while(iterationNumber < 40) {
        geometricCenter(N, K, T);
        change = attribution(0, N, K, P);
        iterationNumber++;
        printf("Fez mais uma iteração %d\n", iterationNumber);
    }

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
    {
        printf("Center: (%.3f , %.3f) : Size: %d\n", geometricCenterX[i], geometricCenterY[i], clusterCurrentPos[i] + 1);
    }
    printf("Iterations : %d\n", iterationNumber - 1);
    int end = 0;
    /*
    for (int p = 0; p < P - 1; p++)
    {
        printf("Envia FIM para %d\n", p + 1);
        // Envia quantidade de números, depois pode-se remover
        MPI_Send(&end, 1, MPI_INT, p + 1,
                 0, MPI_COMM_WORLD);
    }
    */
    MPI_Bcast(&end, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

void attribution_aux(int N, int K, int pos_start, int firstNum)
{
    int change = 0;
    // Este depois pode ser removido e passado como argumento
    // O myID também é desnecessário
    int length_per_process = firstNum;
    // Envia posição a partir da qual o outro deve analisar

    MPI_Bcast(geometricCenterX, K, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(geometricCenterY, K, MPI_FLOAT, 0, MPI_COMM_WORLD);
    int i;
    // Analisa o array do X e do Y, e descobre qual é o melhor cluster.
    for (int i; i < length_per_process; i++)
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
    MPI_Send(&change, 1, MPI_INT, 0,
             0, MPI_COMM_WORLD);
    // Envia which Cluster
    // MPI_Send(wichCluster, i, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Gather(&(wichCluster[pos_start]), length_per_process, MPI_INT, wichCluster, length_per_process, MPI_INT, 0, MPI_COMM_WORLD);
}

// Falta dar free aqui, nos outros processor
void kmeans_aux(int N, int K, int T, int pos_atual)
{
    int iterationNumber = 0;
    while (iterationNumber < number_iteracions)
    {
        int first_num;
        MPI_Bcast(&first_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (first_num == 0)
        {
            printf("FILHO DEVE ACABAR\n");
            break;
        }
        attribution_aux(N, K, pos_atual, first_num);
    }
    printf("Bye\n");
}

// Made with love ❤
