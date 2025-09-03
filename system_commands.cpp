#include "Header.h"

// Execute system command with background support
void executeSystemCommand(const string& input) {
    bool is_background = false;
    string clean_input = input;
    
    // Check if ends with &
    size_t last = clean_input.find_last_not_of(" \t");
    if (last != string::npos && clean_input[last] == '&') {
        is_background = true;
        clean_input = clean_input.substr(0, last);
        // Trim trailing spaces after removing &
        size_t new_last = clean_input.find_last_not_of(" \t");
        if (new_last != string::npos) {
            clean_input = clean_input.substr(0, new_last + 1);
        }
    }
    
    // Handle redirection
    int saved_stdout = -1;
    int saved_stdin = -1;
    
    if (hasRedirection(clean_input)) {
        // Handle output redirection
        bool append = false;
        string output_file = getOutputFile(clean_input, append);
        if (!output_file.empty()) {
            saved_stdout = setupOutputRedirection(output_file, append);
            if (saved_stdout == -1) {
                return;
            }
        }
        
        // Handle input redirection
        string input_file = getInputFile(clean_input);
        if (!input_file.empty()) {
            saved_stdin = setupInputRedirection(input_file);
            if (saved_stdin == -1) {
                if (saved_stdout != -1) {
                    restoreOutput(saved_stdout);
                }
                return;
            }
        }
        
        // Get clean command without redirection
        clean_input = getCleanCommand(clean_input);
    }
    
    // Parse command and arguments
    vector<string> args;
    stringstream ss(clean_input);
    string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }
    
    if (!args.empty()) {
        string command = args[0];
        
        // Check if it's an internal command
        if (isInternalCommand(command)) {
            // Execute internal command directly (no fork needed for internal commands)
            if (command == "echo") {
                echo(clean_input);
            }
            else if (command == "pwd") {
                pwd(clean_input);
            }
            else if (command == "ls") {
                string rest = (clean_input.length() > 2) ? clean_input.substr(2) : "";
                size_t first = rest.find_first_not_of(" \t");
                rest = (first != string::npos) ? rest.substr(first) : "";
                ls(rest);
            }
            else if (command == "pinfo") {
                string rest = (clean_input.length() > 5) ? clean_input.substr(5) : "";
                size_t first = rest.find_first_not_of(" \t");
                rest = (first != string::npos) ? rest.substr(first) : "";
                pinfo(rest);
            }
            else if (command == "search") {
                string filename = (args.size() > 1) ? args[1] : "";
                search(filename);
            }
        }
        else {
            // External command - use execvp
            // Convert to char* array
            vector<char*> argv;
            for (const string& a : args) {
                argv.push_back(const_cast<char*>(a.c_str()));
            }
            argv.push_back(nullptr);
            
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                if (is_background) {
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
                cout << argv[0] << ": command not found" << endl;
                exit(127);
            } else if (pid > 0) {
                if (is_background) {
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
    }
    
    // Restore redirections
    if (saved_stdout != -1) {
        restoreOutput(saved_stdout);
    }
    if (saved_stdin != -1) {
        restoreInput(saved_stdin);
    }
}
