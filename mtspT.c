#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>

#define NUM_THREADS 20 // 线程数量

// 光标移动函数
#define CLEAR() printf("\033[2J")
#define MOVEUP(x) printf("\033[%dA", (x))
#define MOVEDOWN(x) printf("\033[%dB", (x))
#define MOVERIGHT(x) printf("\033[%dC", (x))
#define MOVELEFT(x) printf("\033[%dD", (x))
#define MOVETO(x, y) printf("\033[%d;%dH", (x), (y))
#define RESET_CURSOR() printf("\033[H")
#define HIDE_CURSOR() printf("\033[?25l")
#define SHOW_CURSOR() printf("\033[?25h")
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")

#define HEAD_NORTH 0
#define HEAD_SOUTH 1
#define NOT_TAKEN 2

pthread_cond_t B_N, B_M_T, B_M, B_M_B, B_S,bridge_full;

pthread_mutex_t student_on_bridge_counter_lock;// 桥上人数计数器互斥锁
pthread_mutex_t lock;
pthread_mutex_t B_N_MUTEX_HS, B_S_MUTEX_HN;
int student_on_bridge_counter = 0;
pthread_mutex_t B_STATUS_MUTEX;// 桥的状态互斥锁
int BN_state = NOT_TAKEN;
int BS_state = NOT_TAKEN;
int BMT_state = NOT_TAKEN;
int BMB_state = NOT_TAKEN;// 中上部， 中下部状态

struct student_data {
    int student_id;
    int direction;
    int speed;
    char student_name;
};


struct student_data student_data_array[NUM_THREADS];

void * crossBridge(void *arg);// 线程函数, 同学们过桥
void printBridge();
void student_walk(int x, int y, int Fcolor, int Bcolor, int speed, char student_name, int direction, int where_stop);

// 主函数
int main() {
    int res;
    pthread_t crossBridgeThread[NUM_THREADS];
    void *threadRes;
    int num_thread = NUM_THREADS;
    int index = 0;

    srand(42);

    pthread_mutex_init(&student_on_bridge_counter_lock, 0);

    pthread_mutex_init(&B_STATUS_MUTEX, 0);
    pthread_mutex_init(&lock, 0);

    pthread_cond_init(&bridge_full, 0);
    pthread_cond_init(&B_N, 0);
    pthread_cond_init(&B_S, 0);
    pthread_cond_init(&B_M_T, 0);
    pthread_cond_init(&B_M_B, 0);
    // 画出桥
    printBridge();

    //创建线程
    for(index = 0; index < num_thread; index++) {

        student_data_array[index].student_id = index; // 学生id
        student_data_array[index].direction = rand() % 2; // 随机生成学生过桥方向
        student_data_array[index].student_name = 'A' + index % 26; // 随机生成字母表示同学们的名字
        student_data_array[index].speed = rand() % 5000 + 60000; // 随机生成同学们的速度。

        // 利用student_data 生成同学们过桥的线程。
        res = pthread_create(&crossBridgeThread[index], NULL, crossBridge, (void *) &student_data_array[index]);
        if (res != 0) {
            perror("Thread creation failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    //  等待所有线程结束
    for (index = 0; index < num_thread; index ++) {
        res = pthread_join(crossBridgeThread[index], &threadRes);
        if (res != 0) {
            perror("Thread join failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("\n\n\n\n\nstudents are all gone.\n");
    printf("bridge is closed.\n");

    pthread_mutex_destroy(&student_on_bridge_counter_lock);
    pthread_mutex_destroy(&B_STATUS_MUTEX);
    pthread_cond_destroy(&bridge_full);
    pthread_cond_destroy(&B_N);
    pthread_cond_destroy(&B_S);
    pthread_cond_destroy(&B_M_T);
    pthread_cond_destroy(&B_M_B);

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
    int t_id = my_data->student_id;
    int t_direct = my_data->direction;
    char student_name = my_data->student_name;
    int font_color = t_id % 7;
    int speed = my_data->speed;
    int flag = 0;// 判断桥中上部是否被占用
    int step = 0;

    RESET_CURSOR();
    pthread_mutex_lock(&student_on_bridge_counter_lock);
    student_on_bridge_counter++;
    if (student_on_bridge_counter > 3){
        pthread_cond_wait(&bridge_full, &student_on_bridge_counter_lock);
    }
    pthread_mutex_unlock(&student_on_bridge_counter_lock);
   // 朝南走
    if (t_direct == 1){
        //北段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if (BN_state != NOT_TAKEN || (BMT_state == HEAD_NORTH && BMB_state == HEAD_NORTH) || (BMT_state != NOT_TAKEN && BMB_state != NOT_TAKEN && BS_state == HEAD_NORTH)){
            pthread_cond_wait(&B_N, &B_STATUS_MUTEX);
        }
        BN_state = HEAD_SOUTH;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        for (step = a[1]; step <= c[1] - 3; step++){
            student_walk(c[0] + 2, step, font_color, 7, speed, student_name, 1, c[1]-3);
        }

        // 中段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if (BMT_state != NOT_TAKEN && BMB_state != NOT_TAKEN){// 上下都有人， 堵
            pthread_cond_wait(&B_M, &B_STATUS_MUTEX);
        }
        if (BMT_state == HEAD_SOUTH && BMB_state == NOT_TAKEN && BS_state == HEAD_NORTH ){// 上有人向南， 南边有向北的, 堵在上面
            pthread_cond_signal(&B_M);
            pthread_cond_wait(&B_M_T, &B_STATUS_MUTEX);
            BMT_state = HEAD_SOUTH;
            flag = 1;
        }
        else if (BMB_state == HEAD_SOUTH && BMT_state == NOT_TAKEN && BS_state == HEAD_NORTH ){//下有人向南， 南边有向北的, 堵在下面
            pthread_cond_signal(&B_M);
            pthread_cond_wait(&B_M_B, &B_STATUS_MUTEX);
            BMB_state = HEAD_SOUTH;
            flag = 0;
        }
        else if(BMB_state == HEAD_SOUTH && BMT_state == NOT_TAKEN && BS_state != HEAD_NORTH ){//下有向南走的， 南边没有向北走的， 走上面
            pthread_cond_signal(&B_M);
            BMT_state = HEAD_SOUTH;
            flag = 0;

        }
        else if(BMT_state == HEAD_SOUTH && BMB_state == NOT_TAKEN && BS_state != HEAD_NORTH ){//上有向南走的， 南边没有向北走的， 走上面
            pthread_cond_signal(&B_M);
            BMB_state = HEAD_SOUTH;
            flag = 0;
        }
        else if (BMB_state == NOT_TAKEN && BMT_state == NOT_TAKEN ) {// 上下无人， 走上面
            pthread_cond_signal(&B_M);
            BMT_state = HEAD_SOUTH;
            flag = 1;
        }
        else if (BMT_state == HEAD_NORTH && BMB_state == NOT_TAKEN){//上有人向北，走下面
            pthread_cond_signal(&B_M);
            BMB_state = HEAD_SOUTH;
            flag = 0;
        }
        else if (BMB_state == HEAD_NORTH && BMT_state == NOT_TAKEN ){//下面有人向北， 走上面 
            pthread_cond_signal(&B_M);
            BMT_state = HEAD_SOUTH;
            flag = 1;
        }
        pthread_cond_signal(&B_N);
        BN_state = NOT_TAKEN;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        MOVETO(c[0] + 2, c[1] - 3);
        printf("   ");
        fflush(stdout);
        if (flag == 1){
            for (step = c[1] + 1 ; step <= e[1]-3; step++){
                student_walk(i[0] + 1, step, font_color, 7, speed, student_name, 1, e[1] -3);
            }
        }
        else{
            for (step = c[1] + 1 ; step <= e[1]-3; step++){
                student_walk(h[0] - 1, step, font_color, 7, speed, student_name, 1, e[1] - 3);
            }
        }

        // 南段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if(BS_state != NOT_TAKEN){
            pthread_cond_wait(&B_S, &B_STATUS_MUTEX);
            pthread_cond_signal(&B_M);
        }
        BS_state = HEAD_SOUTH;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        if (flag == 1){
            MOVETO(j[0] + 1, e[1] - 3);
            printf("   ");
            fflush(stdout);
            pthread_mutex_lock(&B_STATUS_MUTEX);
            pthread_cond_signal(&B_M_T);
            BMT_state = NOT_TAKEN;
            pthread_mutex_unlock(&B_STATUS_MUTEX);
        }
        else{
            MOVETO(g[0] - 1, e[1] - 3);
            printf("   ");
            fflush(stdout);
            pthread_mutex_lock(&B_STATUS_MUTEX);
            pthread_cond_signal(&B_M_B);
            BMB_state = NOT_TAKEN;
            pthread_mutex_unlock(&B_STATUS_MUTEX);
        }
        for (step = e[1]; step <= k[1]; step++){
            student_walk(a[0] + 2, step, font_color, 7, speed, student_name, 1, -1);
        }
        pthread_mutex_lock(&B_STATUS_MUTEX);
        pthread_cond_signal(&B_S);
        BS_state = NOT_TAKEN;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
    }
    // 朝北走
    else if (t_direct == 0) {
        if (font_color == 1){// 不允许字体为红色
            font_color = 2;
        }
        // 南段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if (BS_state != NOT_TAKEN || (BMT_state == HEAD_SOUTH && BMB_state == HEAD_SOUTH) || (BMT_state != NOT_TAKEN && BMB_state != NOT_TAKEN && BN_state == HEAD_SOUTH)){
            pthread_cond_wait(&B_S, &B_STATUS_MUTEX);
        }
        BS_state = HEAD_NORTH;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        for (step = k[1]; step >= e[1]; step--){
            student_walk(a[0] + 2, step, font_color, 1, speed, student_name, 0, e[1]);
        }
        usleep(speed);

        // 中段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if (BMT_state != NOT_TAKEN && BMB_state != NOT_TAKEN){// 上下都有人， 堵着
            pthread_cond_wait(&B_M, &B_STATUS_MUTEX);
        }
        else if (BMT_state == HEAD_NORTH && BMB_state == NOT_TAKEN && BN_state == HEAD_SOUTH ){// 上部有向北的， 下部没有人， 北部有向南的， 堵在上面。
            pthread_cond_signal(&B_M);
            pthread_cond_wait(&B_M_T, &B_STATUS_MUTEX);
            BMT_state = HEAD_NORTH;
            flag = 1;
        }
        else if (BMB_state == HEAD_NORTH && BMT_state == NOT_TAKEN && BN_state == HEAD_SOUTH ){ //下部有向北的， 上部没有人， 北部有向南的， 堵在下面。
            pthread_cond_signal(&B_M);
            pthread_cond_wait(&B_M_B, &B_STATUS_MUTEX);
            BMB_state = HEAD_NORTH;
            flag = 0;
        }
        else if (BMB_state == HEAD_NORTH && BMT_state == NOT_TAKEN && BN_state != HEAD_SOUTH ) { //下部有向北的， 上部没有人， 北部没有有向南的， 走上面。

            pthread_cond_signal(&B_M);
            BMT_state = HEAD_NORTH;
            flag = 1;
        }
        else if (BMT_state == HEAD_NORTH && BMB_state == NOT_TAKEN && BN_state != HEAD_SOUTH ) {//上部有向北的， 下部没有人， 北部没有有向南的， 走下面。
            pthread_cond_signal(&B_M);
            BMB_state = HEAD_NORTH;
            flag = 0;
        }
        else if (BMB_state == NOT_TAKEN && BMT_state == NOT_TAKEN ){// 上下没人， 走上面
            pthread_cond_signal(&B_M);
            BMT_state = HEAD_NORTH;
            flag = 1;
        }
        else if (BMT_state == HEAD_SOUTH && BMB_state == NOT_TAKEN){// 上面有向南的， 下面没人， 走下面
            pthread_cond_signal(&B_M);
            BMB_state = HEAD_NORTH;
            flag = 0;
        }
        else if (BMB_state == HEAD_SOUTH && BMT_state == NOT_TAKEN){// 下面有向南的， 上面没人， 走上面
            pthread_cond_signal(&B_M);
            BMT_state = HEAD_NORTH;
            flag = 1;
        }
        else{
            perror("error");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        pthread_mutex_lock(&B_STATUS_MUTEX);
        pthread_cond_signal(&B_S);
        BS_state = NOT_TAKEN;
        pthread_mutex_unlock(&B_STATUS_MUTEX);

        MOVETO(a[0]+2, e[1]),
        printf("    ");
        fflush(stdout);
        if (flag == 1){
            flag = 1;
            for (step = e[1] - 3; step >= i[1] + 1; step--){
                student_walk(i[0] + 1, step, font_color, 1, speed, student_name, 0, i[1] + 1);
            }
        }
        else{
            for (step = e[1] - 3; step >= h[1] + 1; step--){
                student_walk(h[0] - 1, step, font_color, 1, speed, student_name, 0, h[1] + 1);
            }
        }
        usleep(speed);

        // 北段
        pthread_mutex_lock(&B_STATUS_MUTEX);
        if(BN_state != NOT_TAKEN){
            pthread_cond_wait(&B_N, &B_STATUS_MUTEX);
            pthread_cond_signal(&B_M);
        }
        BN_state = HEAD_SOUTH;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
        if (flag == 0){
            pthread_mutex_lock(&B_STATUS_MUTEX);
            pthread_cond_signal(&B_M_B);
            BMB_state = NOT_TAKEN;
            pthread_mutex_unlock(&B_STATUS_MUTEX);
            MOVETO(h[0] - 1, h[1] + 1);
            printf("   ");
            fflush(stdout);
        }
        else{
            pthread_mutex_lock(&B_STATUS_MUTEX);
            pthread_cond_signal(&B_M_T);
            BMT_state = NOT_TAKEN;
            pthread_mutex_unlock(&B_STATUS_MUTEX);
            MOVETO(i[0] + 1, i[1] + 1);
            printf("   ");
            fflush(stdout);
        }
        for (step = c[1] - 3; step >= 0; step--){
            student_walk(a[0] + 2, step, font_color, 1, speed, student_name, 0, -1);
        }
        pthread_mutex_lock(&B_STATUS_MUTEX);
        pthread_cond_signal(&B_N);
        BN_state = NOT_TAKEN;
        pthread_mutex_unlock(&B_STATUS_MUTEX);
    }
    pthread_mutex_lock(&student_on_bridge_counter_lock);
    student_on_bridge_counter--;
    pthread_mutex_unlock(&student_on_bridge_counter_lock);
    pthread_cond_signal(&bridge_full);
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
    fflush(stdout);
    HIDE_CURSOR();
    usleep(100000);
}

void student_walk(int x, int y, int Fcolor, int Bcolor, int speed, char student_name, int direction, int where_stop) {
    if (direction == 1){
        MOVETO(x, y);
        printf("\033[4%d;3%dm%c>>\033[0m", Bcolor, Fcolor, student_name);
        fflush(stdout);
        HIDE_CURSOR();
        usleep(speed);
        MOVETO(x, y);
        printf("   ");
        if (y == where_stop){
            MOVETO(x, y);
            printf("\033[4%d;3%dm%c>>\033[0m", Bcolor, Fcolor, student_name);
        }
    }
    else if(direction == 0){
        MOVETO(x, y);
        printf("\033[4%d;3%dm<<%c\033[0m", Bcolor, Fcolor, student_name);
        fflush(stdout);
        HIDE_CURSOR();
        usleep(speed);
        MOVETO(x, y);
        printf("   ");
        if (y == where_stop){
            MOVETO(x, y);
            printf("\033[4%d;3%dm<<%c\033[0m", Bcolor, Fcolor, student_name);
        }
    }
    else{
        perror("direction must be 1 or 0");
        exit(EXIT_FAILURE);
    }
}



