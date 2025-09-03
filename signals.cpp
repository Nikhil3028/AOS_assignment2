#include "Header.h"

static pid_t foreground_pid = 0;

void set_foreground_process(pid_t pid) {
    foreground_pid = pid;
}

void sigint_handler(int ) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        foreground_pid = 0;
    } else {
        // No foreground process - let readline handle it
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void sigtstp_handler(int ) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        cout << "\n[" << foreground_pid << "] Stopped" << endl;
        foreground_pid = 0;
    }
    // Do nothing if no foreground process is running
}

void setup_signal_handlers() {
    struct sigaction sa;
    
    // Set up SIGINT handler
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    
    // Set up SIGTSTP handler  
    sa.sa_handler = sigtstp_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);
}

void handle_ctrl_d() {
    cout << "exit" << endl;
    exit(0);
}
