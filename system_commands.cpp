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
            execvp(argv[0], argv.data());
            perror(argv[0]);
            exit(1);
        } else if (pid > 0) {
            if (isBackground) {
                cout << "[" << pid << "]" << endl;
            } else {
                wait(NULL);
            }
        } else {
            perror("fork");
        }
    }
}
