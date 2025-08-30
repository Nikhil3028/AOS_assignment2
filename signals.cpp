#include "Header.h"

static pid_t foreground_pid = 0;

void set_foreground_process(pid_t pid) {
    foreground_pid = pid;
}

void sigint_handler(int ) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        foreground_pid = 0;
    }
}

void sigtstp_handler(int ) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        foreground_pid = 0;
    }
}

void setup_signal_handlers() {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}

void handle_ctrl_d() {
    cout << "exit" << endl;
    exit(0);
}
