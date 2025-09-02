#include "Header.h"

void executeSystemCommand(const string& input) {
    bool isBackground = false;
    string cleanInput = input;
    
    // Check if ends with &
    size_t last = cleanInput.find_last_not_of(" \t");
    if (last != string::npos && cleanInput[last] == '&') {
        isBackground = true;
        cleanInput = cleanInput.substr(0, last);
        // trim trailing spaces after removing &
        size_t newLast = cleanInput.find_last_not_of(" \t");
        if (newLast != string::npos) {
            cleanInput = cleanInput.substr(0, newLast + 1);
        }
    }
    
    // Handle redirection
    int saved_stdout = -1;
    int saved_stdin = -1;
    
    if (hasRedirection(cleanInput)) {
        // Handle output redirection
        bool append = false;
        string outputFile = getOutputFile(cleanInput, append);
        if (!outputFile.empty()) {
            saved_stdout = setupOutputRedirection(outputFile, append);
            if (saved_stdout == -1) return;
        }
        
        // Handle input redirection
        string inputFile = getInputFile(cleanInput);
        if (!inputFile.empty()) {
            saved_stdin = setupInputRedirection(inputFile);
            if (saved_stdin == -1) {
                if (saved_stdout != -1) restoreOutput(saved_stdout);
                return;
            }
        }
        
        // Get clean command without redirection
        cleanInput = getCleanCommand(cleanInput);
    }
    
    // Parse command and arguments
    vector<string> args;
    stringstream ss(cleanInput);
    string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }
    
    if (!args.empty()) {
        // Convert to char* array
        vector<char*> argv;
        for (const string& a : args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        argv.push_back(nullptr);
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (isBackground) {
                // Redirect all I/O to /dev/null for background processes
                int devnull_out = open("/dev/null", O_WRONLY);
                if (devnull_out != -1) {
                    dup2(devnull_out, STDOUT_FILENO);
                    dup2(devnull_out, STDERR_FILENO);
                    close(devnull_out);
                }
                int devnull_in = open("/dev/null", O_RDONLY);
                if (devnull_in != -1) {
                    dup2(devnull_in, STDIN_FILENO);
                    close(devnull_in);
                }
            }
            execvp(argv[0], argv.data());
            perror(argv[0]);
            exit(1);
        } else if (pid > 0) {
            if (isBackground) {
                cout << "[" << pid << "]" << endl;
            } else {
                set_foreground_process(pid);
                int status;
                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    // Process was stopped by CTRL-Z, don't reset foreground_pid yet
                    // It will be reset by the signal handler
                } else {
                    set_foreground_process(0);
                }
            }
        } else {
            perror("fork");
        }
    }
    
    // Restore redirections
    if (saved_stdout != -1) restoreOutput(saved_stdout);
    if (saved_stdin != -1) restoreInput(saved_stdin);
}
