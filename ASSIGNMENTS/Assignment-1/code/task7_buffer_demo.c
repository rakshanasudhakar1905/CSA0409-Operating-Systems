#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("buffering demonstration: parent message before fork\n");
    /* Deliberately do not flush: use only to demonstrate duplicated stdio buffer. */
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) exit(0);  /* exit() flushes inherited stdio buffer */
    wait(NULL);
    return 0;
}
