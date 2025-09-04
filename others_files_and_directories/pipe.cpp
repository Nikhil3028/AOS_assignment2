#include "Header.h"

bool hasPipe(const string& input) {
    return input.find("|") != string::npos;
}

vector<string> splitPipe(const string& input) {
    vector<string> commands;
    stringstream ss(input);
    string cmd;
    
    while (getline(ss, cmd, '|')) {
        size_t start = cmd.find_first_not_of(" \t");
        size_t end = cmd.find_last_not_of(" \t");
        if (start != string::npos) {
            commands.push_back(cmd.substr(start, end - start + 1));
        }
    }
    return commands;
}

bool isInternalCommand(const string& cmd) {
    return (cmd == "echo" || cmd == "pwd" || cmd == "ls" || cmd == "pinfo" || cmd == "search");
}

void runInternal(const string& cmd, const string& full) {
    if (cmd == "echo") {
        echo(full);
        cout.flush();
    }
    else if (cmd == "pwd") {
        pwd(full);
        cout.flush();
    }
    else if (cmd == "ls") {
        string args = full.substr(full.find("ls") + 2);
        size_t start = args.find_first_not_of(" \t");
        ls(start != string::npos ? args.substr(start) : "");
        cout.flush();
    }
    else if (cmd == "pinfo") {
        string args = full.substr(full.find("pinfo") + 5);
        size_t start = args.find_first_not_of(" \t");
        pinfo(start != string::npos ? args.substr(start) : "");
        cout.flush();
    }
    else if (cmd == "search") {
        stringstream ss(full);
        string c, file;
        ss >> c >> file;
        
        // If no filename provided, read from stdin (for pipes)
        if (file.empty()) {
            getline(cin, file);
            // Remove any trailing whitespace
            size_t end = file.find_last_not_of(" \t\n\r");
            if (end != string::npos) file = file.substr(0, end + 1);
        }
        search(file);
        cout.flush();
    }
}

void runCommand(const string& cmd, int input_fd, int output_fd) {
    // Set up input redirection
    if (input_fd != -1) {
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2 input");
            exit(1);
        }
        close(input_fd);
    }
    
    // Set up output redirection
    if (output_fd != -1) {
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("dup2 output");
            exit(1);
        }
        close(output_fd);
    }
    
    stringstream ss(cmd);
    string first;
    ss >> first;
    
    if (isInternalCommand(first)) {
        runInternal(first, cmd);
        fflush(stdout);
        fflush(stderr);
        exit(0);
    }
    
    // Parse arguments for external commands
    vector<string> args;
    string token;
    ss.str(cmd);
    ss.clear();
    while (ss >> token) args.push_back(token);
    
    vector<char*> argv;
    for (const string& arg : args) argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr);
    
    execvp(argv[0], argv.data());
    perror(argv[0]);
    exit(1);
}

void executePipe(const vector<string>& commands) {
    if (commands.empty()) return;
    
    int prev_fd = -1;
    
    for (size_t i = 0; i < commands.size(); i++) {
        int pipefd[2] = {-1, -1};
        
        // Create pipe for all but the last command
        if (i < commands.size() - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                if (prev_fd != -1) close(prev_fd);
                return;
            }
        }
        
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            if (pipefd[0] != -1) close(pipefd[0]);
            if (pipefd[1] != -1) close(pipefd[1]);
            if (prev_fd != -1) close(prev_fd);
            return;
        }
        
        if (pid == 0) {
            // Child process
            runCommand(commands[i], prev_fd, pipefd[1]);
            exit(1); // Should never reach here
        }
        
        // Parent process
        if (prev_fd != -1) close(prev_fd);
        if (pipefd[1] != -1) close(pipefd[1]);
        prev_fd = pipefd[0];
        
        // Wait for this process before starting the next
        int status;
        waitpid(pid, &status, 0);
    }
    
    if (prev_fd != -1) close(prev_fd);
}

void executePipeline(const string& input) {
    vector<string> commands = splitPipe(input);
    executePipe(commands);
}
