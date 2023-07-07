#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_ITERATIONS 10000000
#define EPSILON 1e-10

int jacobi(double *x_new, double **A, double *b, double *x, int n, int np) {
    int i, j, k, status, id_seq = 0, pid;;
    double sum, error;

    for (k = 0; k < MAX_ITERATIONS; k++) {
        // Loop para criar os processos filhos
        for(i=1; i<np;i++){
            pid = fork();
            if ( pid == 0){id_seq = i;break;} 
        }
        
        int start_index = id_seq * (n / np);
        int end_index = (id_seq == np - 1) ? n : (id_seq + 1) * (n / np);

        for (i = start_index; i < end_index; i++) {
            sum = 0;
            for (j = 0; j < n; j++){ 
                if (j != i){sum += A[i][j] * x[j];}
            }
            x_new[i] = (b[i] - sum) / A[i][i];
        }
       
        // Sincronização dos processos filhos
        for (i = 1; i < np; i++) {
            if (id_seq == 0) {wait(&status);}
            if (id_seq == i) {exit(0);}
        }

        error = 0;
        for (i = 0; i < n; i++){error += fabs(x[i] - x_new[i]);}// Verifica a convergência a cada iteração
        if (error < EPSILON) {break;}// Condição de convergência
        memcpy(x, x_new, n * sizeof(double));//// Copia o novo vetor para o vetor antigo
    }
    memcpy(x, x_new, n * sizeof(double));// Copia o vetor final para o vetor X
    return k;// Retorna o número de iterações
}

void populadados(double **A,double *B,double *X,int n){
    int i, j;
    // Preencher a matriz A com valores aleatórios
    for (i = 0; i < n; i++) {
        double soma = 0.0;
        for (j = 0; j < n; j++) {
            if (i != j) {
                A[i][j] = 1; // Valores entre 0 e 10 com uma casa decimal
                soma += abs(A[i][j]);
            }
        }
        A[i][i] = soma + 1.0; // Valor na diagonal principal para tornar a matriz diagonal dominante
    }
    // Preencher o vetor B com valores aleatórios
    for (i = 0; i < n; i++) {
        B[i] = 1; // Valores entre 0 e 10 com uma casa decimal
    }
    for (i = 0; i < n; i++)  
        X[i] = 0.0; // Inicialize o vetor X com zeros
}

void printVetor(double *v, int n){
    for (int i = 0; i < n; i++){printf("%.2f ", v[i]);}
    printf("\n");
}
/*-----------------------------------------------*/
int main(int argc, char **argv){
    int n, np, i, j, inter;
    double **A, *B, *X, tempo_execucao;
    struct timeval start, end;
        
    printf("Digite o tamanho da matriz: ");
    scanf("%d", &n);
    printf("Digite o número de processos (1 ou +): ");
    scanf("%d", &np);
    
    //Pegar todos os Dados de todas as tabelas de 1 a 4 processadores
    //int *matriz=(int *)malloc(3 * sizeof(int));
    //matriz[0]=800,matriz[1]=1000, matriz[2]=1200;
    //for(int count = 0; count<3; count++){\
    //for(int np = 1; np<=4; np++){
    //n=matriz[count];

    //gettimeofday(&start, NULL);// Início do cronômetro
    X = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor X
    B = (double *)malloc(n * sizeof(double));// Aloca memória para o vetor B
    A = (double **)malloc(n * sizeof(double *));// Aloca memória para a matriz
    for (i = 0; i < n; i++){
        A[i] = (double *)malloc(n * sizeof(double));// Aloca memória para os elementos da matriz
    }
    populadados(A, B, X, n);         

    // Aloca memória compartilhada para o vetore novo
    int shm_x_new = shmget(IPC_PRIVATE, n * sizeof(double), 0600 | IPC_CREAT);
    double *x_new = (double *)shmat(shm_x_new, NULL, 0);

    gettimeofday(&start, NULL);// Início do cronômetro

    inter = jacobi(x_new, A, B, X, n, np);

    // Libera a memória compartilhada alocada
    shmdt(x_new);
    shmctl(shm_x_new, IPC_RMID, NULL);

    gettimeofday(&end, NULL);// Fim do cronômetro

    // Cálculo do tempo de execução em segundos
    tempo_execucao = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Convergência alcançada após:\n %d iterações, \n Utilizando %d Processos, \n Tempo de execução: %.6f segundos,\n para a matriz de tamanho %d.\n", inter, np, tempo_execucao, n);
    //printf("Resultado final do vetor X:\n");
    //printVetor(X, n);    
    // Libera a memória alocada para os elementos da matriz
    for (i = 0; i < n; i++){free(A[i]);}
    // Libera a memória alocada para a matriz e Vetores
    free(A);
    free(B);
    free(X);
    //}
    //}
}