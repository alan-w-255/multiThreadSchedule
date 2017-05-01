#include<stdio.h>
#include<pthread.h>
#include<Windows.h>
#include<semaphore.h>
#pragma comment(lib, "pthreadVC2.lib")>

#define HEAD_NORTH 0
#define HEAD_SOUTH 1
#define NUM_THREADS 8



void CLEAR(){
	system("cls");
}
void MOVETO(int x, int y) {
    COORD cursorPosition;
    cursorPosition.X = y;
    cursorPosition.Y = x;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}
void RESET_CURSOR(){
    MOVETO(0, 0);
}
void HIDE_CURSOR() {
    MOVETO(-1, -1);
}
void SET_TEXT_COLOR(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

sem_t B;
sem_t B_N;
sem_t B_M;
sem_t B_S;
sem_t draw_lock;

sem_t GO_UP_LOCK;
int is_up_taken = 0;


void * crossBridge(void *arg);
void printBridge();
void student_walk(int x, int y, int speed, char student_name, int direction, int where_stop);


struct student_data {
    int student_id;
    int direction;
    int speed;
    char student_name;
};

struct student_data student_data_array[NUM_THREADS];

int main() {
    int res;
    pthread_t crossBridgeThread[NUM_THREADS];
    void *threadRes;
    int num_thread = NUM_THREADS;
    int index = 0;
    srand(22);

    //初始化信号量
    res = sem_init(&B, 0, 2);
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
    res = sem_init(&GO_UP_LOCK, 0, 1);
    if (res != 0) {
        perror("Semaphore go up lock initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    res = sem_init(&draw_lock, 0, 1);
    if (res != 0) {
        perror("Semaphore go up lock initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    // 画出桥
    printBridge();

    //create threads
    for(index = 0; index < num_thread; index++) {
        student_data_array[index].student_id = index;
        student_data_array[index].speed = 100;
        student_data_array[index].direction = rand() % 2;
        student_data_array[index].student_name = 'A' + index % 26;
        res = pthread_create(&crossBridgeThread[index], NULL, crossBridge, (void *) &student_data_array[index]);
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
    sem_destroy(&GO_UP_LOCK);
    exit(EXIT_SUCCESS);
}

void *crossBridge(void * arg) {
    int a[2] = {4, 1};
    //int b[2] = {7, 1};
    int c[2] = {4, 12};
    //int d[2] = {7, 27};
    int e[2] = {4, 27};
    //int f[2] = {7, 27};
    int g[2] = {9, 27};
    int h[2] = {9, 12};
    int i[2] = {3, 12};
    int j[2] = {3, 27};
    int k[2] = {4, 40};
    //int l[2] = {7, 40};

    struct student_data * my_data;
    my_data = (struct student_data *) arg;
    int t_direct = my_data->direction;
    char student_name = my_data->student_name;
    int speed = my_data->speed;
    int flag = 0;
    int step = 0;

    RESET_CURSOR();

    sem_wait(&B);// 获得过桥资格ss
    // 朝南走
    if (t_direct == 1){

        sem_wait(&B_N);
        for (step = a[1]; step <= c[1] - 3; step++){
            student_walk(c[0] + 2, step, speed, student_name, 1, c[1]-3);
        }

        sem_wait(&B_M);
        sem_post(&B_N);
        MOVETO(c[0] + 2, c[1] - 3);
        printf("   ");
        sem_wait(&GO_UP_LOCK);
        if (is_up_taken == 0) {
            is_up_taken = 1;
            sem_post(&GO_UP_LOCK);
            flag = 1;
            for (step = c[1] + 1 ; step <= e[1]-3; step++){
                student_walk(i[0] + 1, step,  speed, student_name, 1, e[1] -3);
            }
        }
        else{
            sem_post(&GO_UP_LOCK);
            for (step = c[1] + 1 ; step <= e[1]-3; step++){
                student_walk(h[0] - 1, step,  speed, student_name, 1, e[1] - 3);
            }
        }

        // 南段
        sem_wait(&B_S);
        sem_wait(&GO_UP_LOCK);
        if (flag == 1) {
            is_up_taken = 0;
            sem_post(&GO_UP_LOCK);
            MOVETO(j[0] + 1, e[1] - 3);
            printf("   ");
        }
        else {
            sem_post(&GO_UP_LOCK);
            MOVETO(g[0] - 1, e[1] - 3);
            printf("   ");
        }
        sem_post(&B_M);
        for (step = e[1]; step <= k[1]; step++){
            student_walk(a[0] + 2, step, speed, student_name, 1, -1);
        }
        sem_post(&B_S);
    }
    // 朝北走
    else {
        sem_wait(&B_S);//进入第一段的资格
        for (step = k[1]; step >= e[1]; step--){
            student_walk(a[0] + 2, step, speed, student_name, 0, e[1]);
        }

        sem_wait(&B_M); // 进入中段的资格
        sem_wait(&GO_UP_LOCK);
        sem_post(&B_S);// 释放进入第一段的资格
        MOVETO(a[0]+2, e[1]),
        printf("    ");
        if (is_up_taken == 0) {
            is_up_taken = 1;
            sem_post(&GO_UP_LOCK);
            flag = 1;
            for (step = e[1] - 3; step >= i[1] + 1; step--){
                student_walk(i[0] + 1, step, speed, student_name, 0, i[1] + 1);
            }
        }
        else{
            sem_post(&GO_UP_LOCK);
            for (step = e[1] - 3; step >= h[1] + 1; step--){
                student_walk(h[0] - 1, step, speed, student_name, 0, h[1] + 1);
            }
        }

        sem_wait(&B_N);
        sem_wait(&GO_UP_LOCK);
        if (flag == 1) {
            is_up_taken = 0; // 
            sem_post(&GO_UP_LOCK);
            MOVETO(i[0] + 1, i[1] + 1);
            printf("   ");
        }
        else {
            sem_post(&GO_UP_LOCK);
            MOVETO(h[0] - 1, h[1] + 1);
            printf("   ");
        }

        sem_post(&B_M);// 释放进入中段的资格
        for (step = c[1] - 3; step > 0; step--){
            student_walk(a[0] + 2, step, speed, student_name, 0, -1);
        }
        sem_post(&B_N);
    }
    sem_post(&B);
    pthread_exit(NULL);
}

/**
0         1         2         3         4    
11234567890123456789012345678901234567890
2
3           i--------------j
4a__________|              |____________k
5           c              e
6
7___________d              f____________l
8b          |              |
9           h--------------g
0
a = [4, 1]
b = [7, 1]
c = [4, 12]
d = [7, 27]
e = [4, 27]
f = [7, 27]
g = [9, 27]
h = [9, 12]
i = [3, 12]
j = [3, 27]
k = [4, 40]
l = [7, 40]
*/
void printBridge() {
    CLEAR();
    RESET_CURSOR();
    printf("N<<- the bridge is open. ->>S\n");
    RESET_CURSOR();
    MOVETO(3,12);
    printf("__________________");
    MOVETO(4,1);
    printf("___________|                |____________");
    MOVETO(7,1);
    printf("___________                  ____________");
    MOVETO(8, 12);
    printf("|                |");
    MOVETO(9, 12);
    printf("------------------");
}

void student_walk(int x, int y, int speed, char student_name, int direction, int where_stop) {
    if (direction == 1){
        sem_wait(&draw_lock);
        MOVETO(x, y);
        SET_TEXT_COLOR(8);
        printf("%c>>", student_name);
        HIDE_CURSOR();
        sem_post(&draw_lock);
        Sleep(speed);
        sem_wait(&draw_lock);
        MOVETO(x, y);
        printf("   ");
        HIDE_CURSOR();
        sem_post(&draw_lock);
        if (y == where_stop){
            sem_wait(&draw_lock);
            MOVETO(x, y);
            printf("%c>>", student_name);
            sem_post(&draw_lock);
        }
    }
    else if(direction == 0){
        sem_wait(&draw_lock);
        MOVETO(x, y);
        SET_TEXT_COLOR(8);
        printf("<<%c", student_name);
        HIDE_CURSOR();
        sem_post(&draw_lock);
        Sleep(speed);
        sem_wait(&draw_lock);
        MOVETO(x, y);
        HIDE_CURSOR();
        sem_post(&draw_lock);
        printf("   ");
        if (y == where_stop){
            sem_wait(&draw_lock);
            MOVETO(x, y);
            printf("<<%c", student_name);
            sem_post(&draw_lock);
        }
    }
    else{
        perror("direction must be 1 or 0");
        exit(EXIT_FAILURE);
    }
}