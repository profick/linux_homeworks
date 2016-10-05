#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void print_signal_info(int signum, siginfo_t* signal_info, void* context) {
    int signal_number;
    if (signum == SIGUSR1) {
        signal_number = 1;
    } else {
        signal_number = 2;
    }

    printf("SIGUSR%d from %d\n", signal_number, signal_info->si_pid);
    exit(0);
}

int main() {
    struct sigaction signal_action;
    sigset_t usr_mask;

    //when handler will work with one of the SIGUSR, another one will be blocked.
    sigemptyset(&usr_mask);
    sigaddset(&usr_mask, SIGUSR1);
    sigaddset(&usr_mask, SIGUSR2);

    //setup action
    signal_action.sa_flags = SA_SIGINFO;
    signal_action.sa_sigaction = print_signal_info;
    signal_action.sa_mask = usr_mask;

    //set actions on signals
    sigaction(SIGUSR1, &signal_action, NULL);
    sigaction(SIGUSR2, &signal_action, NULL);
    sleep(10);


    printf("No signals were caught\n");
    return 0;
}
