#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include <bits/stl_vector.h>

const int BUF_SIZE = 1024;

using namespace std;

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
        token = strtok(NULL, "\t\n\r\a");
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
            string str = "stopped by signal " + WTERMSIG(status);
            str += ".\n";
            write(STDOUT_FILENO, &str, str.length());
        }
    }
    return 1;
}

//int launch_pipe(char** args, vector<int> pipes) {
//    int fds[2];
//    pipe(fds);
//    pid_t pid;
//
//    if (fork() == 0) {
//        dup2(fds[0], 0);
//        close(fds[1]);
//        execvp()
//    }
//}

//vector<int> count_pipes(char** args) {
//    vector<int> pipes;
//    int index = 0;
//    while (args[index] != NULL) {
//        if (*args[index] == '|') {
//            pipes.push_back(index++);
//        }
//    }
//    return pipes;
//}

//int execute(char** args) {
//    vector<int> pipes = count_pipes(args);
//    if (pipes.size() == 0) {
//        launch_single(args);
//    } else {
//        launch_pipe(args, pipes);
//    }
//}


void loop() {
    char** args;
    int status;

    do {
        write(STDOUT_FILENO, "$ ", 2);
        args = split_line(read_line());
        status = launch_single(args);

        free(args);
    } while (status);
}


int main() {

    loop();
    return 0;
}
