#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>

const int BUF_SIZE = 1024;

using namespace std;

void handler(int signum, siginfo_t* siginfo, void* context) {

}


char* read_line() {
    //allocate memory for buffer

    int bufsize = BUF_SIZE;
    char *buffer = (char *) malloc(sizeof(char) * bufsize);
    //check if allocate success
    if (!buffer) {
        string str = "ERROR: cannot allocate memory.\n";
        write(STDOUT_FILENO, &str, str.length());
        exit(EXIT_FAILURE);
    }

    //reading from STDIN up till bufsize bytes to the buffer
    int retIn = 0;
    int readed = 0;
    while (retIn = read(STDIN_FILENO, buffer + readed, bufsize - readed)) {
        readed += retIn;
        if (buffer[readed - 1] == '\n') {
            break;
        }
    }



    //
    if (readed == bufsize) {
        bufsize *= 2;
        buffer = (char*) realloc(buffer, bufsize);
        while (retIn = read(STDIN_FILENO, buffer + readed, bufsize - readed)) {
            readed += retIn;
        }

    }


    //
    if (!buffer) {
        string str = "ERROR: cannot allocate memory.\n";
        write(STDOUT_FILENO, &str, str.length());
        exit(EXIT_FAILURE);
    }

    return buffer;

}

char** split_line(char* text) {

    int buffer_size = BUF_SIZE;
    int position = 0;
    char* token;
    char** arguments = (char **) malloc(buffer_size * sizeof(char*));

    if (!arguments) {
        string str = "ERROR: cannot allocate memory.\n";
        write(STDOUT_FILENO, &str, str.length());
        exit(EXIT_FAILURE);
    }

    token = strtok(text, " \t\n\r\a");
    while (token != NULL) {

        arguments[position++] = token;
        if (position >= buffer_size) {
            buffer_size *= 2;
            arguments = (char**) realloc(arguments, buffer_size);
            if (!arguments) {
                string str = "ERROR: cannot allocate memory.\n";
                write(STDOUT_FILENO, &str, str.length());
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\n\r\a");
    }
    arguments[position] = NULL;

    return arguments;
}

int launch_single(char** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        //child
        if (execvp(args[0], args) == -1) {
            string str = "ERROR: cannot execute the command.\n";
            write(STDOUT_FILENO, &str, str.length());
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        //forking error
        string str = "ERROR: cannot fork.\n";
        write(STDOUT_FILENO, &str, str.length());
        exit(EXIT_FAILURE);
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if (WIFSIGNALED(status)) {
//            string str = "stopped by signal " + WTERMSIG(status);
//            str += ".\n";
//            write(STDOUT_FILENO, &str, str.length());
        }
    }
    return 1;
}

int launch_pipe(char** args, vector<int> pipes) {
    vector<int> pid_array;
    int pipefd[2];
    int oldpipefd[2];
    pid_t pid;
    char** command = (char**) malloc(BUF_SIZE * sizeof(char*));
    for (int i = 0; i < pipes[0]; i++) {
        command[i] = args[i];
    }
    command[pipes[0]] = NULL;
    pipe(pipefd);
    if (pid = fork() == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        execvp(command[0], command);
    } else {
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    pid_array.push_back(pid);
    oldpipefd[0] = pipefd[0];
    oldpipefd[1] = pipefd[1];

    for (int i = 1; i < pipes.size(); i++) {

        pipe(pipefd);
        if (pid = fork()) {
            close(oldpipefd[0]);
            close(oldpipefd[1]);
        } else {
            dup2(oldpipefd[0], 0);
            close(oldpipefd[1]);
            dup2(pipefd[1], 1);

            for (int j = 0; j < pipes[i + 1] - pipes[i] - 1; j++) {
                command[j] = args[pipes[i] + j + 1];
            }
            command[pipes[i + 1] - pipes[i] - 1] = NULL;
            execvp(command[0], command);

        }
        pid_array.push_back(pid);
        oldpipefd[0] = pipefd[0];
        oldpipefd[1] = pipefd[1];

    }

    for (int i = pipes[pipes.size() - 1] + 1; ; i++) {
        command[i - (pipes[pipes.size() - 1] + 1)] = args[i];
        if (args[i] == NULL) {
            break;
        }
    }

    pipe(pipefd);
    if (pid = fork()) {
        close(pipefd[0]);
        close(pipefd[1]);
    } else {
        dup2(oldpipefd[0], 0);
        close(oldpipefd[1]);
        execvp(command[0], command);
    }

    pid_array.push_back(pid);

    free(command);
    return 1;
}


vector<int> count_pipes(char** args) {
    vector<int> pipes;
    int index = 0;
    while (args[index] != NULL) {
        if (*args[index] == '|') {
            pipes.push_back(index);

        }

        index++;
    }
    return pipes;
}

int execute(char** args) {
    vector<int> pipes = count_pipes(args);
    if (pipes.size() == 0) {
        launch_single(args);
    } else {

        launch_pipe(args, pipes);
    }
    return 1;
}


void loop() {
    char** args;
    int status;

    do {
        write(STDOUT_FILENO, "$ ", 2);
        char* line = read_line();
        args = split_line(line);
        status = execute(args);

        free(args);
    } while (status);
}


int main() {

    loop();
    return 0;
}
