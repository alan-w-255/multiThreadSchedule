#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

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

void printChar(int i);


int main() {
    CLEAR();
    int i;
    for(i = 0; i < 15; i++) {
        printChar(i);
    }
    RESET_CURSOR();
}

void printChar(int i) {
        printf("%d*", i);
        MOVEDOWN(1);
        fflush(stdout);
        sleep(1);
}
