#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static int write_all(int fd, const char *buf, size_t count) {
    size_t done = 0;
    while (done < count) {
        ssize_t w = write(fd, buf + done, count - done);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (w == 0) return -1;
        done += (size_t)w;
    }
    return 0;
}

long copy_file(const char *src, const char *dst, size_t bufsize) {
    char buf[8192];
    if (bufsize == 0 || bufsize > sizeof(buf)) {
        errno = EINVAL;
        return -1;
    }
    int in = open(src, O_RDONLY);
    if (in < 0) return -1;
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { close(in); return -1; }

    long calls = 0;
    ssize_t n;
    while (1) {
        do {
            n = read(in, buf, bufsize);
            calls++;
        } while (n < 0 && errno == EINTR);

        if (n == 0) break;
        if (n < 0 || write_all(out, buf, (size_t)n) < 0) {
            close(in);
            close(out);
            return -1;
        }
    }
    if (close(in) < 0 || close(out) < 0) return -1;
    return calls;
}

int main(void) {
    FILE *f = fopen("records.dat", "w");
    if (!f) { perror("fopen"); return 1; }
    for (int i = 0; i < 10000; i++) fputc('A' + i % 26, f);
    if (fclose(f) != 0) { perror("fclose"); return 1; }

    size_t sizes[] = {1, 512, 4096};
    for (int i = 0; i < 3; i++) {
        long count = copy_file("records.dat", "copy.dat", sizes[i]);
        if (count < 0) { perror("copy_file"); return 1; }
        printf("buffer %4zu bytes: %5ld read() calls\\n", sizes[i], count);
    }
    unlink("records.dat");
    unlink("copy.dat");
    return 0;
}
