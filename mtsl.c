#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

// 常量定义
    #define NUM_THREADS 8

// 定义光标移动函数
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

// 信号量定义
    sem_t B;
    sem_t B_N;
    sem_t B_M_T;
    sem_t B_M_B;
    sem_t B_S;
    sem_t DRAW_LOCK;
    sem_t MIDDLE_STATE_LOCK;

// 全局共享资源
    int MT_STATE = -1;
    int MB_STATE = -1;

// 线程信息
    struct thread_data {
        int thread_id;
        int direction;
        char student_name;
    };

struct thread_data thread_data_array[NUM_THREADS];

// 主函数
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
        res = sem_init(&B_M_T, 0, 1);
        if (res != 0) {
            perror("Semaphore B_M_T initialization failed.\n");
            exit(EXIT_FAILURE);
        }
        res = sem_init(&B_M_B, 0, 1);
        if (res != 0) {
            perror("Semaphore B_M_B initialization failed.\n");
            exit(EXIT_FAILURE);
        }
        res = sem_init(&DRAW_LOCK, 0, 1);
        if (res != 0) {
            perror("Semaphore DRAW_LOCK initialization failed.\n");
            exit(EXIT_FAILURE);
        }
        res = sem_init(&MIDDLE_STATE_LOCK, 0, 1);
        if (res != 0) {
            perror("Semaphore DRAW_LOCK initialization failed.\n");
            exit(EXIT_FAILURE);
        }


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
// 销毁信号量
    sem_destroy(&B);
    sem_destroy(&B_N);
    sem_destroy(&B_M_T);
    sem_destroy(&B_M_B);
    sem_destroy(&B_S);
    sem_destroy(&DRAW_LOCK);
    sem_destroy(&MIDDLE_STATE_LOCK);
    exit(EXIT_SUCCESS);
}

void *crossBridge(void * arg) {
    struct thread_data * my_data;
    my_data = (struct thread_data *) arg;
    //int t_id = my_data->thread_id;
    int t_direct = my_data->direction;
    char student_name = my_data->student_name;
    int flag = 0; // 判断该线程走的中上部 0 还是中下部 1；
    RESET_CURSOR();

    sem_wait(&B);// 获得过桥资格, 最多只能允许三人通过
    // 朝南走
    if (t_direct == 1){
        sem_wait(&B_N);
        sem_wait(&DRAW_LOCK);
        MOVETO(5, 4);
        printf("%c->", student_name);
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_LOCK);

        sem_wait(&MIDDLE_STATE_LOCK);
        sem_wait(&DRAW_LOCK);
        MOVETO(5, 4);
        printf("   ");
        sem_post(&B_N);
        if (MT_STATE == -1) {// 中上部没有人。
            if (MB_STATE == -1 || MB_STATE == 1) {
                //如果中下部没人或者有人向北走， 进入中上部。
                sem_wait(&B_M_T);
                MT_STATE = 0;
                flag = 0;
                MOVETO(4, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else if (MB_STATE == 0) { // 如果中下部有向南走的, 从中下部走
                sem_post(&MIDDLE_STATE_LOCK);
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else {
                printf("Error: unknow MB_STATE: %d", MB_STATE);
                fflush(stdout);
            }
        }
        else if(MT_STATE == 0) { // 中上部有向南的, 堵塞这条线程
            sem_post(&MIDDLE_STATE_LOCK);
            sem_wait(&B_M_T);
            MB_STATE = 0;
            flag = 0;
            MOVETO(4, 12);
            printf("%c->", student_name);
            fflush(stdout);
            sleep(1);
        }
        else if (MT_STATE == 1) { // 中上部有向北的，走中下部
            if (MB_STATE == 0) { // 中下部有向南的,堵塞。 
                sem_post(&MIDDLE_STATE_LOCK);
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else if (MB_STATE == -1) { // 中下部没有人。走中下部.
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else {
                printf("Error: unknow MB_STATE: %d", MB_STATE);
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }
        else {
            printf("Error: unknow MT_STATE: %d/n", MT_STATE);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        sem_post(&DRAW_LOCK);
        sem_post(&MIDDLE_STATE_LOCK);

        sem_wait(&B_S);
        sem_wait(&DRAW_LOCK);
        if (flag == 0) {
            sem_post(&B_M_T);
            MT_STATE = -1;
            MOVETO(4, 12);
            printf("   ");

        }
        else {
            sem_post(&B_M_B);
            MT_STATE = -1;
            MOVETO(6, 12);
            printf("   ");
        }
        MOVETO(5, 20);
        printf("%c->", student_name);
        fflush(stdout);
        sleep(1);
        MOVETO(5, 20);
        printf("   ");
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_LOCK);
        sem_post(&B_S);
    }
    // 朝北走
    else {
        sem_wait(&B_S);
        sem_wait(&DRAW_LOCK);
        MOVETO(5, 4);
        printf("<-%c", student_name);
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_LOCK);

        sem_wait(&MIDDLE_STATE_LOCK);
        sem_wait(&DRAW_LOCK);
        MOVETO(5, 20);
        printf("   ");
        sem_post(&B_S);
        if (MT_STATE == -1) {// 中上部没有人。
            if (MB_STATE == -1 || MB_STATE == 0) {
                //如果中下部没人或者向南走， 进入中上部。
                sem_wait(&B_M_T);
                MT_STATE = 1;
                flag = 0;
                MOVETO(4, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else if (MB_STATE == 1) { // 如果中下部有向北走的, 堵塞.向下走.
                sem_post(&MIDDLE_STATE_LOCK);
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else {
                printf("Error: unknow MB_STATE: %d", MB_STATE);
                fflush(stdout);
            }
        }
        else if(MT_STATE == 0) { // 中上部有向南的, 堵塞这条线程
                sem_post(&MIDDLE_STATE_LOCK);
                sem_wait(&B_M_T);
                MT_STATE = 1;
                flag = 0;
                MOVETO(4, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
        }
        else if (MT_STATE == 1) { // 中上部有向北的，走中下部
            if (MB_STATE == 0) { // 中下部有向南的,堵塞。 
                sem_post(&MIDDLE_STATE_LOCK);
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else if (MB_STATE == -1) { // 中下部没有人。走中下部.
                sem_wait(&B_M_B);
                MB_STATE = 0;
                flag = 1;
                MOVETO(6, 12);
                printf("%c->", student_name);
                fflush(stdout);
                sleep(1);
            }
            else {
                printf("Error: unknow MB_STATE: %d", MB_STATE);
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }
        else {
            printf("Error: unknow MT_STATE: %d/n", MT_STATE);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        sem_post(&DRAW_LOCK);
        sem_post(&MIDDLE_STATE_LOCK);

        sem_wait(&B_S);
        sem_wait(&DRAW_LOCK);
        if (flag == 0) {
            sem_post(&B_M_T);
            MOVETO(4, 12);
            printf("   ");

        }
        else {
            sem_post(&B_M_B);
            MOVETO(6, 12);
            printf("   ");
        }
        MOVETO(5, 20);
        printf("%c->", student_name);
        fflush(stdout);
        sleep(1);
        MOVETO(5, 20);
        printf("   ");
        fflush(stdout);
        sleep(1);
        sem_post(&DRAW_LOCK);
        sem_post(&B_S);
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
