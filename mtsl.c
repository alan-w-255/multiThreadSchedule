#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

#define HEAD_NORTH 0
#define HEAD_SOUTH 1
#define NUM_THREADS 8

#define NORTH_PART 0
#define SOUTH_PART 1
#define MIDDLE_TOP_PART 2 
#define MIDDLE_BOTTOM_PART 3


#define CLEAR() printf("\033[2J")
#define MOVEUP(x) printf("\033[%dA", (x))
#define MOVEDOWN(x) printf("\033[%dB", (x))
#define MOVERIGHT(x) printf("\033[%dC", (x))
#define MOVELEFT(x) printf("\033[%dD", (x))
#define MOVETO(x, y) printf("\033[%d;%dH", (x), (y))
#define RESET_CURSOR() printf("\033[H")
#define HIDE_CURSOR() printf("\033[?25l", (x))
#define SHOW_CURSOR() printf("\033[?25h")
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")

void * crossBridge(void *arg);
void printBridge();

sem_t B;
sem_t B_N;
sem_t B_M;
sem_t B_S;

sem_t DRAW_KEY;
sem_t DRAW_UP_KEY;
int is_up_taken = 0;



struct thread_data {
    int thread_id;
    int direction;
    char student_name;
};

struct thread_data thread_data_array[NUM_THREADS];

int main() {
    int res;
    pthread_t crossBridgeThread[NUM_THREADS];
    void *threadRes;
    int num_thread = NUM_THREADS;
    int index = 0;
    srand(22);

    //初始化信号量
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
    res = sem_init(&DRAW_KEY, 0, 1);
    if (res != 0) {
        perror("Semaphore DRAW_KEY initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    res = sem_init(&DRAW_UP_KEY, 0, 1);

    // 画出桥
    printBridge();

    //create threads
    for(index = 0; index < num_thread; index++) {
        thread_data_array[index].thread_id = index;
        thread_data_array[index].direction = rand() % 2;
        thread_data_array[index].student_name = 'A' + index % 26;
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

    printf("\n\n\n\n\nstudents are all gone.\n");
    printf("bridge is closed.\n");
    sem_destroy(&B);
    sem_destroy(&B_N);
    sem_destroy(&B_M);
    sem_destroy(&B_S);
    sem_destroy(&DRAW_KEY);
    sem_destroy(&DRAW_UP_KEY);
    exit(EXIT_SUCCESS);
}

void *crossBridge(void * arg) {

    struct thread_data * my_data;
    my_data = (struct thread_data *) arg;
    //int t_id = my_data->thread_id;
    int t_direct = my_data->direction;
    char student_name = my_data->student_name;
    int flag = 0;
    RESET_CURSOR();

    sem_wait(&B);// 获得过桥资格ss
    // 朝南走
    if (t_direct == 1){

        sem_wait(&B_N);
        //printf("%d in N", t_id);
        sem_wait(&DRAW_KEY);
        MOVETO(5, 4);
        printf("%c->", student_name);
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_KEY);


        sem_wait(&B_M);
        //printf("%d in M", t_id);
        sem_wait(&DRAW_KEY);
        MOVETO(5, 4);
        printf("   ");
        sem_post(&B_N);
        sem_wait(&DRAW_UP_KEY);
        if (is_up_taken == 0) {
            is_up_taken = 1;
            flag = 1;
            MOVETO(4, 12);
            printf("%c->", student_name);
            fflush(stdout);
            sleep(1);
        }
        else{
            MOVETO(6, 12);
            printf("%c->", student_name);
            fflush(stdout);
            sleep(1);
        }
        sem_post(&DRAW_UP_KEY);
        sem_post(&DRAW_KEY);


        sem_wait(&B_S);
        //printf("%d in S", t_id);
        sem_wait(&DRAW_KEY);
        sem_wait(&DRAW_UP_KEY);
        if (flag == 1) {
            MOVETO(4, 12);
            printf("   ");
            is_up_taken = 0;
        }
        else {
            MOVETO(6, 12);
            printf("   ");
        }
        sem_post(&B_M);
        sem_post(&DRAW_UP_KEY);
        MOVETO(5, 20);
        printf("%c->", student_name);
        fflush(stdout);
        sleep(1);
        MOVETO(5, 20);
        printf("   ");
        sleep(1);
        sem_post(&DRAW_KEY);
        sem_post(&B_S);
    }
    // 朝北走
    else {

        sem_wait(&B_S);//进入第一段的资格
        sem_wait(&DRAW_KEY);
        MOVETO(5, 20);
        printf("<-%c", student_name);
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_KEY);



        sem_wait(&B_M); // 进入中段的资格
        sem_wait(&DRAW_KEY);
        MOVETO(5,20);
        printf("   ");
        sem_post(&B_S);// 释放进入第一段的资格
        sem_wait(&DRAW_UP_KEY);
        if (is_up_taken == 0) {
            is_up_taken = 1;
            flag = 1;
            MOVETO(4, 12);
            printf("<-%c", student_name);
        }
        else{
            MOVETO(6, 12);
            printf("<-%c", student_name);
        }
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_UP_KEY);
        sleep(1);
        sem_post(&DRAW_KEY);



        sem_wait(&B_N);
        sem_wait(&DRAW_KEY);
        sem_wait(&DRAW_UP_KEY);
        if (flag == 1) {
            MOVETO(4, 12);
            printf("   ");
            is_up_taken = 0; // 
        }
        else {
            MOVETO(6, 12);
            printf("   ");
        }
        sem_post(&B_M);// 释放进入中段的资格
        sem_post(&DRAW_UP_KEY);
        MOVETO(5, 4);
        printf("<-%c", student_name);
        fflush(stdout);
        sleep(1);
        MOVETO(5, 4);
        printf("   ");
        sleep(1);
        sem_post(&DRAW_KEY);
        sem_post(&B_N);


    }
    sem_post(&B);
    pthread_exit(NULL);
}

/**
0 print a bridge like this.
1
2       ________
3_______|      |________
4
5_______        ________
6       |      |
7       --------
*/

void printBridge() {
    CLEAR();
    RESET_CURSOR();
    printf("N<-- the bridge is open. -->S\n");
    RESET_CURSOR();
    MOVEDOWN(2);
    MOVERIGHT(8);
    printf("_________");
    RESET_CURSOR();
    MOVEDOWN(3);
    printf("________|       |________");
    RESET_CURSOR();
    MOVEDOWN(5);
    printf("________         ________");
    RESET_CURSOR();
    MOVEDOWN(6);
    MOVERIGHT(8);
    printf("|       |");
    RESET_CURSOR();
    MOVEDOWN(7);
    MOVERIGHT(8);
    printf("_________");
    fflush(stdout);
    sleep(1);
}







