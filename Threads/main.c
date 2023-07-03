#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4

//Function executed by each thread
void *thread_function(void *thread_arg) {
    int thread_id = *(int *)thread_arg;
    //printf("Hello from thread %d\n", thread_id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i;
    // Create threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }
    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; i++) {pthread_join(threads[i], NULL);}
    printf("All threads have finished execution.\n");
    return 0;
}
