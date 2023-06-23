#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_ITERATIONS 1000
#define EPSILON 1e-10

int jacobi(double *x_new, double *x_old, double **A, double *b, double *x, int n){
    int i, j, k;
    double sum, error;

    memcpy(x_old, x, n * sizeof(double));// Copia o vetor inicial para o vetor antigo

    // Itera até a convergência ou até o número máximo de iterações ser atingido
    for (k = 0; k < MAX_ITERATIONS; k++){
        // Atualiza os elementos do vetor
        for (i = 0; i < n; i++){
            sum = 0;
            for (j = 0; j < n; j++)
                if (j != i)
                    sum += A[i][j] * x_old[j];
            x_new[i] = (b[i] - sum) / A[i][i];
        }

        // Verifica a convergência
        error = 0;
        for (i = 0; i < n; i++)
            error += fabs(x_old[i] - x_new[i]);

        // Condição de convergência
        if (error < EPSILON){
            printf("Convergência alcançada em %d iterações.\n", k);
            break;
        }

        // Copia o novo vetor para o vetor antigo
        memcpy(x_old, x_new, n * sizeof(double));
    }
    
    memcpy(x, x_new, n * sizeof(double));// Copia o vetor final para o vetor X
    
    return k;// Retorna o número de iterações
}

int jacobi_parallel(double *x_new, double *x_old, double **A, double *b, double *x, int n, int np, int pid) {
    int i, j, k, status;
    double sum, error;

    memcpy(x_old, x, n * sizeof(double));// Copia o vetor inicial para o vetor antigo
    
    // Loop para as iterações do método Jacobi
    for (k = 0; k < MAX_ITERATIONS; k++) {
        if (pid == 0) {
            // Processo filho
            // Atualiza os elementos do vetor para o processo filho
            for (i = i - 1; i < n; i += (np - 1)) {
                sum = 0;
                for (j = 0; j < n; j++) {
                    if (j != i) {
                        sum += A[i][j] * x_old[j];
                    }
                }
                x_new[i] = (b[i] - sum) / A[i][i];
            }

            // Libera a memória compartilhada alocada para os vetores do processo filho
            shmdt(x_new);
            shmdt(x_old);

            // Finaliza o processo filho
            exit(0);
        } else {
            // Processo pai
            for (i = 1; i < np; i++) 
                wait(&status);// Aguarda a conclusão dos processos Pai

            // Verifica a convergência
            error = 0;
            for (i = 0; i < n; i++) {
                sum = 0;
                for (j = 0; j < n; j++) 
                    if (j != i) 
                        sum += A[i][j] * x_old[j];
                      
                x_new[i] = (b[i] - sum) / A[i][i];
                error += fabs(x_new[i] - x_old[i]);
            }

            // Condição de convergência
            if (error < EPSILON) {
                k++;
                printf("Convergência alcançada após %d iterações.\n", k);
                break;
            }
            memcpy(x_old, x_new, n * sizeof(double));// Copia o novo vetor para o vetor antigo// Copia o novo vetor para o vetor antigo
        }
    }
    memcpy(x, x_old, n * sizeof(double));// Copia o resultado final do vetor para o vetor de saída

    return k;// Retorna o número de iterações
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
    int n, np, i, j, inter;
    double **A, *B, *X, tempo_execucao;
    struct timeval start, end;

    // if ( argc != 3 ){
    //     printf("%s <num_ele> <num_proc>\n", argv[0]);
    //     exit(0);
    // }
    
    do{
        // Tamanho da matriz
        printf("Digite o tamanho da matriz(4 ou 9 ou 13): ");
        scanf("%d", &n);
        if (n != 4 && n != 9 && n != 13)
            printf("Tamanho inválido.\n");
        else break;
    }while(!(n == 4 || n == 9 || n == 13));
    
    printf("Digite o número de processos (1 ou +): ");
    scanf("%d", &np);

    //gettimeofday(&start, NULL);// Início do cronômetro

    // n = atoi(argv[1]);
    // np = atoi(argv[2]);
    
    X = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor X
    B = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor B
    A = (double **)malloc(n * sizeof(double *));// Aloca memória para a matriz
    for (i = 0; i < n; i++)
        A[i] = (double *)malloc(n * sizeof(double));// Aloca memória para os elementos da matriz

    populadados(A, B, X, n);         

    if (np == 1){
        double *x_new = (double *)malloc(n * sizeof(double)), *x_old = (double *)malloc(n * sizeof(double));;// Aloca memória para os vetores novo e antigo
        
        gettimeofday(&start, NULL);// Início do cronômetro

        inter = jacobi(x_new, x_old, A, B, X, n);

        free(x_new);// Libera a memória alocada para os vetores
        free(x_old);// Libera a memória alocada para os vetores
    }
    else{ 
        // Aloca memória compartilhada para os vetores novo e antigo
        int shm_x_new = shmget(IPC_PRIVATE, n * sizeof(double), 0600 | IPC_CREAT);
        int shm_x_old = shmget(IPC_PRIVATE, n * sizeof(double), 0600 | IPC_CREAT);
        double *x_new = (double *)shmat(shm_x_new, NULL, 0), *x_old = (double *)shmat(shm_x_old, NULL, 0);

        // Loop para criar os processos filhos
        int id_seq = 0, pid;
        for(i=1; i<np;i++){
            pid = fork();
            if ( pid == 0){
                id_seq = i;
                break;
            } 
        }

        gettimeofday(&start, NULL);// Início do cronômetro

        inter = jacobi_parallel(x_new, x_old, A, B, X, n, np, pid);

        // Libera a memória compartilhada alocada para os vetores
        shmdt(x_new);
        shmdt(x_old);
        shmctl(shm_x_new, IPC_RMID, NULL);
        shmctl(shm_x_old, IPC_RMID, NULL);
    }
    gettimeofday(&end, NULL);// Fim do cronômetro

    // Cálculo do tempo de execução em segundos
    tempo_execucao = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Resultado final do vetor X:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%.2f ", X[i]);
    }
    printf("\n");
    printf("interacoes = %d\n", inter);
    printf("Tempo de execução: %.6f segundos\n", tempo_execucao);
    
    // Libera a memória alocada para os elementos da matriz
    for (i = 0; i < n; i++)
        free(A[i]);
    // Libera a memória alocada para a matriz e Vetores
    free(A);
    free(B);
    free(X);
}