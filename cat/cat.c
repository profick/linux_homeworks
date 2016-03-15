#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 256

int main(int argc, char *argv[])
{
    char buffer[BUF_SIZE];
    ssize_t retIn, retOut;
    int fd;
    fd = open (argv[1], O_RDONLY);
    if (fd == -1) {
        perror("wrong input");
        return -1;
    }
    while ((retIn = read(fd, &buffer, BUF_SIZE)) > 0 || retIn == -1) {
        if (retIn == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("error in reading");
                return -1;
            }
        }
        retOut = 0;
        ssize_t tmp = 0;
        while (retOut < retIn) {
            tmp = write(STDOUT_FILENO, &buffer + retOut, retIn - retOut);
            if (tmp == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    perror("error in writing");
                    return -1;
                }

            }
            retOut = retOut + tmp;
        }
    }
    return 0;
}
