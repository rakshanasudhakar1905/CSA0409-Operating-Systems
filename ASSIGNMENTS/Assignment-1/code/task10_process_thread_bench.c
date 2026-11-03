#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#define COUNT 1000
static void *thread_work(void *arg) { (void)arg; return NULL; }

static double elapsed(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) / 1e9;
}

int main(void) {
    struct timespec a, b;
    int made = 0;
    clock_gettime(CLOCK_MONOTONIC, &a);
    for (int i = 0; i < COUNT; i++) {
        pid_t p = fork();
        if (p < 0) { perror("fork"); break; }
        if (p == 0) _exit(0);
        if (waitpid(p, NULL, 0) < 0) { perror("waitpid"); return 1; }
        made++;
    }
    clock_gettime(CLOCK_MONOTONIC, &b);
    printf("processes created and reaped: %d, elapsed: %.6f seconds\n",
           made, elapsed(a, b));

    pthread_t *threads = calloc(COUNT, sizeof(*threads));
    if (!threads) return 1;
    clock_gettime(CLOCK_MONOTONIC, &a);
    made = 0;
    for (int i = 0; i < COUNT; i++) {
        int rc = pthread_create(&threads[i], NULL, thread_work, NULL);
        if (rc != 0) { fprintf(stderr, "pthread_create failed at %d: %d\n", i, rc); break; }
        made++;
    }
    for (int i = 0; i < made; i++) pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &b);
    printf("threads created and joined: %d, elapsed: %.6f seconds\n",
           made, elapsed(a, b));
    free(threads);
    return 0;
}
