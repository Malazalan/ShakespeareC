#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

long int MAX_ATTEMPTS = 1000000000000;
int MAX_THREADS = 500;
char SHAKESPEARE[12] = "SHAKESPEARE";
pthread_mutex_t MAX_MUTEX;

void howCorrect(char* attempt, int* bestN, char* bestA, long int tries, int threadNum) {
    int correct = 0;
    for (int i = 0; i < 11; i++) {
        if (attempt[i] != SHAKESPEARE[i]) {
            break;
        }
        correct++;
    }
    pthread_mutex_lock(&MAX_MUTEX);
    if (correct > *bestN) {
        *bestN = correct;
        strncpy(bestA, attempt, 12);
        printf("[THREAD %d]\t%s - %d correct\t~%ld tries\n", threadNum, attempt, correct, (tries*MAX_THREADS));
    }
    pthread_mutex_unlock(&MAX_MUTEX);
}

void generateAttempt(char* attempt) {
    for (int i = 0; i < 11; i++) {
        attempt[i] = arc4random_uniform(26) + 65;
    }
    attempt[11] = '\0';
}

struct ThreadArgs {
    char* bestAttmpt;
    int* bestNum;
    int threadNum;
};

void *attemptWrapper(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    char attempt[12];
    for (long int i = 0; i < MAX_ATTEMPTS; i++) {
        generateAttempt(attempt);
        howCorrect(attempt, thread_args->bestNum, thread_args->bestAttmpt, i, thread_args->threadNum);
    }
}

int main(void) {
    char bestAttempt[12] = {0};
    int bestNum = 0;
    char attempt[12] = {0};
    printf("===UNIFORM WITH THREADS===\n");
    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        struct ThreadArgs *threadArgs = malloc(sizeof (struct ThreadArgs));
        threadArgs->bestAttmpt = bestAttempt;
        threadArgs->bestNum = &bestNum;
        threadArgs->threadNum = i;
        int threadError = pthread_create(&threads[i], NULL, attemptWrapper, (void*) threadArgs);
        if (threadError > 0) {
            printf("Failed to create thread %d\n", i);
            exit(1);
        }
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
