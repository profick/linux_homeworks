#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>

#define BUF_SIZE 1
#define READ 0
#define WRITE 1

using namespace std;

void write_to_stdin(char *str);
void failure_exit(char* message);
string read_line();
string skip_whitespaces(string str);
int run(string cmd, int input, int isFirst, int isLast);
void split(string cmd);
int execute_command(int input, int isFirst, int isLast);
void waiting_for_childs();


vector<pid_t> processes;
vector<string> args;
string command_buffer = "";
pid_t pid;
int notEOF = 1;

void handler(int signum, siginfo_t* siginfo, void* context) {
    for (int i = 0; i < processes.size(); i++) {
        kill (processes[i], signum);
    }
}

int main() {
    struct sigaction act;
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);

    while (notEOF) {
        write_to_stdin((char*)"$ ");

        int input = 0;
        int isFirst = 1;
        int isLast = 0;
        string command = read_line();
        string cmd = command;
        size_t next = command.find_first_of('|');
        while (next != string::npos) {
            cmd = command.substr(0, next);
            command.erase(0, next + 1);
            input = run(cmd, input, isFirst, isLast);
            next = command.find_first_of('|');
            isFirst = 0;
        }
        isLast = 1;
        run(command, input, isFirst, isLast);
        waiting_for_childs();
    }
    return 0;
}


int run(string cmd, int input, int isFirst, int isLast) {
    split(cmd);
    if (!args.empty()) {
        if (strcmp(args[0].c_str(), "exit") == 0) {
            exit(0);
        }
        return execute_command(input, isFirst, isLast);
    }
}

int execute_command(int input, int isFirst, int isLast) {
    int pipefd[2];
    pipe(pipefd);
    pid = fork();

    if (pid == 0) {
        // child
        if (isFirst == 1 && isLast == 0 && input == 0) {
            //First command of pipe
            dup2(pipefd[WRITE], STDOUT_FILENO);
        } else if (isFirst == 0 && isLast == 0 && input != 0) {
            //Middle command of pipe
            dup2(input, STDIN_FILENO);
            dup2(pipefd[WRITE], STDOUT_FILENO);
        } else {
            //Last command of pipe
            dup2(input, STDIN_FILENO);
        }
        vector<char*> c_args;
        for (int i = 0; i < args.size(); i++) {
            c_args.push_back((char*) args[i].c_str());
        }
        c_args.push_back(NULL);
        if (execvp(c_args[0], c_args.data()) == -1) {
            failure_exit((char*) "Error! \n");
        }
    }
    processes.push_back(pid);
    if (input != 0) {
        close(input);
    }
    close(pipefd[WRITE]);
    if (isLast == 1) {
        close(pipefd[READ]);
    }

    return pipefd[READ];
}

void waiting_for_childs() {
    for (int i = 0; i < processes.size(); ++i) {
        int status;
        waitpid(processes[i], &status, 0);
    }
    processes.clear();
}

//-----------------------------------------------------
void write_to_stdin(char *str) {
    size_t i = strlen(str);
    size_t length = i;
    while (i > 0) {
        i -= write(STDOUT_FILENO, str + length - i, i);
    }

}

void failure_exit(char* message) {
    write_to_stdin(message);
    _exit(0);
}

string read_line() {
    char buffer[BUF_SIZE];
    size_t pos = 0;
    int retIn = 0;

    if (command_buffer.empty() || (pos = command_buffer.find_first_of('\n')) == string::npos) {
        //reading a command from STDIN
        while (retIn = read(STDIN_FILENO, buffer, BUF_SIZE)) {
            if (retIn != -1) {
                command_buffer.append(buffer, retIn);
            }

            if ((pos = command_buffer.find_first_of('\n')) != string::npos) {
                break;
            }

            buffer[0] = '\0';
        }

        if (retIn == 0) {
            notEOF = 0;
        }

    }

    
    string command = command_buffer.substr(0, pos);
    command_buffer.erase(0, pos + 1);
    return command;
}

string skip_whitespaces(string str) {
    int i = 0;
    for (;isspace(str[i]) && i < str.length(); i++);
    str.erase(0,i);
    return str;
}

void split(string cmd) {
    args.clear();
    cmd = skip_whitespaces(cmd);
    size_t next = cmd.find_first_of(' ');
    int i = 0;
    while (next != string::npos) {
        args.push_back(cmd.substr(0, next));

        cmd.erase(0,next + 1);
        ++i;
        cmd = skip_whitespaces(cmd);
        next = cmd.find_first_of(' ');
    }
    if (!cmd.empty()) {
        args.push_back(cmd);
        ++i;
    }
}
