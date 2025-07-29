#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_RESOURCES 5
#define NUM_THREADS 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t resource_mutex;

//function to decrease resources
int decrease_count(int thread_number, int count){
    pthread_mutex_lock(&resource_mutex); //lock before critical section

    if (available_resources < count) {
        pthread_mutex_unlock(&resource_mutex); //unlock
        return -1;
    }else {
        available_resources -= count;
        printf("The thread %d has acquired %d resources and %d more resources are available.\n", thread_number,count,available_resources);
    }
    pthread_mutex_unlock(&resource_mutex); //unlock after critical section
    return 0;
}

int increase_count(int thread_number, int count){
    pthread_mutex_lock(&resource_mutex);

    available_resources+=count;
    printf("The thread %d has relased %d resources and %d resources are now available.\n", thread_number, count, available_resources);

    pthread_mutex_unlock(&resource_mutex);
    return 0;
}

//thread function
void* thread_function(void* arg){
    int thread_number = *(int*)arg;
    int request_count = 1; //each thread requests 1 resource

    if(decrease_count(thread_number,request_count) == 0){
        sleep(1);
        increase_count(thread_number, request_count);
    } else {
        printf("Thread %d could not acquire enough resources.\n", thread_number);
    }
    pthread_exit(NULL);
}



int main(){
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    //for mutex
    if(pthread_mutex_init(&resource_mutex, NULL)!=0){
        fprintf(stderr, "mutex init failed\n");
        return EXIT_FAILURE;
    }

    //create threads
    for(int i =0; i<NUM_THREADS; i++){
        thread_ids[i] = i;
        if(pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0){
            fprintf(stderr, "error creating thread %d\n", i);
        }
    }

    //join threads
    for(int i =0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("All threads have finished execution. Available resources: %d\n", available_resources);

    pthread_mutex_destroy(&resource_mutex);
    return 0;
}