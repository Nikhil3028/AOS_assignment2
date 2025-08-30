#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main() {
    cout << "Starting simple pipe test" << endl;
    
    int pipefd[2];
    pipe(pipefd);
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1: echo hello
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        cout << "hello world" << endl;
        cout.flush();
        exit(0);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2: grep hello
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp("grep", "grep", "hello", NULL);
        exit(1);
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    cout << "Test complete" << endl;
    return 0;
}
