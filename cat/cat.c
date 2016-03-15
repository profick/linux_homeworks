#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 256

int main()
{
    char buffer[BUF_SIZE];
    ssize_t retIn, retOut;

    while ((retIn = read(STDIN_FILENO, &buffer, BUF_SIZE)) > 0) {
        retOut = write(STDOUT_FILENO, &buffer, (ssize_t) retIn);
        while (retOut < retIn) {
            retOut = retOut + write(STDOUT_FILENO, &buffer + retIn - retOut, retIn - retOut);
        }
    }

    return 0;
}
