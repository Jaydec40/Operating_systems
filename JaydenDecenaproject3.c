/*
Jayden Decena
Project 3: thread race conditions
add: mutex functions 
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common.h"
#include "common_threads.h"

int max;
volatile int counter = 0; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Initialize mutex

void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread)
    printf("%s: begin [addr of i: %p, ] [addr of counter: %p]\n", letter, &i, (void*) &counter);
    for (i = 0; i < max * 1000; i++) {
        Pthread_mutex_lock(&mutex); // Lock mutex before accessing counter
        counter = counter + 1; //shared: only one
        Pthread_mutex_unlock(&mutex); // Unlock mutex after updating counter
    }
    printf("%s: done\n", letter);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: main - first ");
        exit(1);
    }
    max = atoi(argv[1]);
    pthread_t p1, p2;
    printf("main: begin [counter = %d] [%p]\n", counter, (void*) &counter);
    Pthread_create(&p1, NULL, mythread, "A");
    Pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("main: done [counter: %d] [should: %d]\n", counter, max * 2 * 1000);
    return 0;
}
