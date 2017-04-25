#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

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

void printBridge();

// 主函数
int main() {

    int res;
    pthread_t crossBridgeThread[NUM_THREADS];
    void *threadRes;
    int num_thread = NUM_THREADS;
    int index = 0;
    srand(22);


    // 画出桥
    printBridge();

    //create threads
    // waiting for crossBridge threads finish
// 销毁信号量
    exit(EXIT_SUCCESS);
}





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