#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

void initialize(int N, int K, int T);
void initialize2(int N, int K, int T);
void testIniatialize(int N, int K, int T);
int attribution(int init, int N, int K, int P);
void geometricCenter(int N, int K, int T);
void geometricCenter2(int N, int K, int T);
void kmeans(int N, int K, int T, int P);
void kmeans_aux(int N, int K, int T, int pos_atual);
void attribution_aux(int N, int K, int myId, int firstNum);
