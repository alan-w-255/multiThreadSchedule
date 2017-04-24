#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

#define HEAD_SOUTH 1
#define HEAD_NORTH 0
#define NUM_THREADS 8

void * crossBridge(void *arg);
void printBridge();

sem_t B;
sem_t B_N;
sem_t B_M;
sem_t B_S;



struct thread_data {
    int thread_id;
    int direction;
};

struct thread_data thread_data_array[NUM_THREADS];

int main() {

    int res;
    pthread_t crossBridgeThread[NUM_THREADS];
    void *threadRes;
    int num_thread = NUM_THREADS;
    int index = 0;

    srand(42);


    res = sem_init(&B, 0, 3);
    if (res != 0) {
        perror("Semaphore B initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    res = sem_init(&B_N, 0, 1);
    if (res != 0) {
        perror("Semaphore B_N initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    res = sem_init(&B_S, 0, 1);
    if (res != 0) {
        perror("Semaphore B_S initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    res = sem_init(&B_M, 0, 2);
    if (res != 0) {
        perror("Semaphore B_M initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("the bridge is open.\n");
    //create threads
    for(index = 0; index < num_thread; index++) {
        thread_data_array[index].thread_id = index;
        thread_data_array[index].direction = rand() % 2;
        res = pthread_create(&crossBridgeThread[index], NULL, crossBridge, (void *) &thread_data_array[index]);
        if (res != 0) {
            perror("Thread creation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    // waiting for crossBridge threads finish
    for (index = 0; index < num_thread; index ++) {
        res = pthread_join(crossBridgeThread[index], &threadRes);
        if (res != 0) {
            perror("Thread join failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("students are all gone.\n");
    printf("bridge is closed.\n");
    sem_destroy(&B);
    sem_destroy(&B_N);
    sem_destroy(&B_M);
    sem_destroy(&B_S);
    exit(EXIT_SUCCESS);
}

void *crossBridge(void * arg) {

    struct thread_data * my_data;
    my_data = (struct thread_data *) arg;
    int t_id = my_data->thread_id;
    int t_direct = my_data->direction;
    sem_wait(&B);// 获得过桥资格ss
    if (t_direct == 0){
        printf("student %d says, 'I wanna cross the bridge to the south.'\n", t_id);
        sleep(1);

        sem_wait(&B_N);
        printf("student %d says, 'I have cross the north part of the bridge.'\n", t_id);
        sleep(1);
        sem_post(&B_N);

        sem_wait(&B_M);
        printf("student %d says, 'I have cross the middle part of the bridge.'\n", t_id);
        sleep(1);
        sem_post(&B_M);

        sem_wait(&B_S);
        printf("student %d says, 'I have cross the south part of the bridge.'\n", t_id);
        sem_post(&B_S);

        printf("student %d says, 'I have cross the whole bridge finally!'\n", t_id);
    }
    else {
        printf("student %d says, 'I wanna cross the bridge to the north.'\n", t_id);
        sleep(1);

        sem_wait(&B_S);
        printf("student %d says, 'I have cross the south part of the bridge.'\n", t_id);
        sem_post(&B_S);

        sem_wait(&B_M);
        printf("student %d says, 'I have cross the middle part of the bridge.'\n", t_id);
        sleep(1);
        sem_post(&B_M);

        sem_wait(&B_N);
        printf("student %d says, 'I have cross the north part of the bridge.'\n", t_id);
        sleep(1);
        sem_post(&B_N);

        printf("student %d says, 'I have cross the whole bridge finally!'\n", t_id);
    }
    sem_post(&B);
    pthread_exit(NULL);
}