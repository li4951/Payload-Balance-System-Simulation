#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

int main() {
 
    while (1) {
        pid_t pid;
        if((pid = fork()) < 0) {
            perror("fork:");
        } else if (pid == 0) {
            printf("reboot SysCtrlNode\n");
            execl("./SysCtrlNode", "./SysCtrlNode", "NULL");
            perror("execlv:");
            exit(1);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}
