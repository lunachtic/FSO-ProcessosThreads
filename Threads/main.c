#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_ITERATIONS 100000
#define EPSILON 1e-10

struct PARAMETROS {
    double *x_new;
    double **A;
    double *b;
    double *x;
    int n;
    int start_index;
    int end_index;
    int nt;
    int id_seq;
};

void *thread_function(void *arg) {
    struct PARAMETROS *data = (struct PARAMETROS *)arg;
    double *x_new = data->x_new;
    double **A = data->A;
    double *b = data->b;
    double *x = data->x;
    int n = data->n;
    int nt = data->nt;
    int id_seq = data->id_seq;

    double sum;
    int start_index = id_seq * (n / nt);
    int end_index = (id_seq == nt - 1) ? n : (id_seq + 1) * (n / nt);

    for (int i = start_index; i < end_index; i++) {
        sum = 0;
        for (int j = 0; j < n; j++) {
            if (j != i) {
                sum += A[i][j] * x[j];
            }
        }
        x_new[i] = (b[i] - sum) / A[i][i];
    }
    pthread_exit(NULL);
}

int jacobi_paralelo(double *x_new, double **A, double *b, double *x, int n, int nt) {
    int i, k;
    double sum, error;
    
    pthread_t *threads = malloc(nt * sizeof(pthread_t));
    struct PARAMETROS *threadData = malloc(nt * sizeof(struct PARAMETROS));

    for (k = 0; k < MAX_ITERATIONS; k++) {
        for (i = 0; i < nt; i++) {
            threadData[i].x_new = x_new;
            threadData[i].A = A;
            threadData[i].b = b;
            threadData[i].x = x;
            threadData[i].n = n;
            threadData[i].nt = nt;
            threadData[i].id_seq = i;

            pthread_create(&threads[i], NULL, thread_function, (void *)&threadData[i]);
        }

        for (i = 0; i < nt; i++) {pthread_join(threads[i], NULL);}

        error = 0;
        for (i = 0; i < n; i++){error += fabs(x[i] - x_new[i]);}// Verifica a convergência a cada iteração
        if (error < EPSILON) {
            break;}// Condição de convergência
        memcpy(x, x_new, n * sizeof(double));//// Copia o novo vetor para o vetor antigo
    }
    memcpy(x, x_new, n * sizeof(double));

    free(threads);
    free(threadData);
    return k;
}

void populadados(double **A,double *B,double *X,int n){
    int i, j;
    if (n == 4){
        // Inicialize os valores de A, b para a matriz 4x4
        double matrixA[4][4] = {
            {10, 1, 0, 0},
            {2, 12, 1, 0},
            {0, 3, 15, 1},
            {0, 0, 4, 20}};
        double vectorB[4] = {7, 8, 9, 10};
        for (i = 0; i < n; i++) {
            B[i] = vectorB[i]; // Preencha o vetor B com os valores definidos
            for (j = 0; j < n; j++) 
                A[i][j] = matrixA[i][j]; // Preencha a matriz A com os valores definidos
        }
    }
    else if (n == 9){
        // Inicialize os valores de A, b para a matriz 9x9
        double matrixA[9][9] = {{10, -1, 2, 0, 0, 0, 0, 0, 0},
                                {1, 15, -2, 1, 0, 0, 0, 0, 0},
                                {0, -3, 20, -1, 2, 0, 0, 0, 0},
                                {0, 0, -2, 18, -3, 1, 0, 0, 0},
                                {0, 0, 0, 1, 25, -4, 2, 0, 0},
                                {0, 0, 0, 0, -2, 22, -1, 3, 0},
                                {0, 0, 0, 0, 0, -1, 30, -3, 1},
                                {0, 0, 0, 0, 0, 0, -2, 28, -2},
                                {0, 0, 0, 0, 0, 0, 0, -1, 35}};
        double vectorB[9] = {9, 5, 8, 2, 7, 3, 6, 4, 1};
        for (i = 0; i < n; i++){
            B[i] = vectorB[i]; // Preencha o vetor B com os valores definidos
            for (j = 0; j < n; j++)
                A[i][j] = matrixA[i][j]; // Preencha a matriz A com os valores definidos
        }
    }
    else if (n == 13){
        // Inicialize os valores de A, b para a matriz 13x13
        double matrixA[13][13] = {{25, -2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {1, 30, -3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, -4, 35, -2, 3, 0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, -3, 40, -4, 4, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, -2, 45, -5, 5, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, -1, 50, -6, 6, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, -2, 55, -7, 7, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, -3, 60, -8, 8, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, -4, 65, -9, 9, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, -3, 70, -10, 10, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 75, -11, 11},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 80, -12},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 85}};
        double vectorB[13] = {11, 8, 13, 7, 12, 6, 10, 5, 9, 4, 8, 3, 7};
        for (i = 0; i < n; i++){
            B[i] = vectorB[i]; // Preencha o vetor B com os valores definidos
            for (j = 0; j < n; j++)
                A[i][j] = matrixA[i][j]; // Preencha a matriz A com os valores definidos
        }
    }
    for (i = 0; i < n; i++)  
        X[i] = 0.0; // Inicialize o vetor X com zeros
}
/*-----------------------------------------------*/
int main(int argc, char **argv)
{
    int n, nt, i, j, inter;
    double **A, *B, *X, tempo_execucao;
    struct timeval start, end;
    
    int *matriz=(int *)malloc(3 * sizeof(int));
    matriz[0]=4,matriz[1]=9, matriz[2]=13;

    // if ( argc != 3 ){
    //     printf("%s <num_ele> <num_proc>\n", argv[0]);
    //     exit(0);
    // }
    
    do{//Pegar tamanho da matriz e numero de processadores de acordo com o Usuario
        printf("Digite o tamanho da matriz(4 ou 9 ou 13): ");
        scanf("%d", &n);
        if (n != 4 && n != 9 && n != 13){printf("Tamanho inválido.\n");}
        else {break;}
    }while(!(n == 4 || n == 9 || n == 13));
    printf("Digite o número de processos (1 ou +): ");
    scanf("%d", &nt);

    //gettimeofday(&start, NULL);// Início do cronômetro

    // n = atoi(argv[1]);
    // nt = atoi(argv[2]);

    //Pegar todos os Dados de todas as tabelas de 1 a 4 processadores
    //for(int count = 0; count<3; count++){\
    //for(int nt = 1; nt<=4; nt++){
    //n=matriz[count];
    
    X = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor X
    B = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor B
    A = (double **)malloc(n * sizeof(double *));// Aloca memória para a matriz
    for (i = 0; i < n; i++){
        A[i] = (double *)malloc(n * sizeof(double));// Aloca memória para os elementos da matriz
    }
    populadados(A, B, X, n);         
    double *x_new = (double *)malloc(n * sizeof(double));// Aloca memória para os vetores novo
    gettimeofday(&start, NULL);// Início do cronômetro
    inter = jacobi_paralelo(x_new, A, B, X, n, nt);
    free(x_new);// Libera a memória alocada para os vetores
    gettimeofday(&end, NULL);// Fim do cronômetro

    // Cálculo do tempo de execução em segundos
    tempo_execucao = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Convergência alcançada após %d iterações, com %d Processadores, para a matriz de tamanho %d.\n", inter, nt, n);
    printf("Resultado final do vetor X:\n");
    for (int i = 0; i < n; i++){printf("%.2f ", X[i]);}
    printf("\n");
    printf("interacoes = %d\n", inter);
    printf("Tempo de execução: %.6f segundos\n", tempo_execucao);
    
    // Libera a memória alocada para os elementos da matriz
    for (i = 0; i < n; i++){free(A[i]);}
    // Libera a memória alocada para a matriz e Vetores
    free(A);
    free(B);
    free(X);
    //}
    //}   
}