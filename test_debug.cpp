#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
using namespace std;

int main() {
    cout << "Testing basic pipe functionality..." << endl;
    
    // Test 1: Simple pipe - ls | grep .cpp
    cout << "Test 1: ls | grep .cpp" << endl;
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1: ls
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execlp("ls", "ls", NULL);
        perror("ls");
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2: grep .cpp
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp("grep", "grep", ".cpp", NULL);
        perror("grep");
        exit(1);
    }
    
    // Parent
    close(pipefd[0]);
    close(pipefd[1]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    cout << "Test complete." << endl;
    return 0;
}
