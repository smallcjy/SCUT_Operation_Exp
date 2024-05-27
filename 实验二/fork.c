#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    // 创建第一个子进程
    pid1 = fork();
    if (pid1 < 0) {
        // fork() 失败
        perror("fork failed");
        return 1;
    } else if (pid1 == 0) {
        // 第一个子进程
        while (1) {
            printf("b\n");
            sleep(1); // 模拟工作，间隔 1 秒
        }
    } else {
        // 父进程继续创建第二个子进程
        pid2 = fork();
        if (pid2 < 0) {
            // fork() 失败
            perror("fork failed");
            return 1;
        } else if (pid2 == 0) {
            // 第二个子进程
            while (1) {
                printf("c\n");
                sleep(1); // 模拟工作，间隔 1 秒
            }
        } else {
            // 父进程
            while (1) {
                printf("a\n");
                sleep(1); // 模拟工作，间隔 1 秒
            }
            // 父进程等待子进程结束（这部分实际不会执行到，因为前面的 while 循环是无限的）
            wait(NULL);
            wait(NULL);
        }
    }
    return 0;
}
