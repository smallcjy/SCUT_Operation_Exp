#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int
main(void) {
    int fd[2];
    pipe(fd);
    //先接受P1
    if(fork() == 0) {
        //子进程1
        char buf[] = {"Child 1 is sending a message!\n"};
        write(fd[1], buf, sizeof(buf));
        exit(0);
    }else {
        char buf[100];
        read(fd[0], buf, sizeof(buf));
        printf(buf);
    }
    //接受P2
    if(fork() == 0) {
        //子进程2
        char buf[] = {"Child 2 is sending a message!\n"};
        write(fd[1], buf, sizeof(buf));
        exit(0);
    }
    else {
        char buf[100];
        read(fd[0], buf, sizeof(buf));
        printf(buf);
    }
    return 0;
}
