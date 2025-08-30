#include "Header.h"

bool hasRedirection(const string& input) {
    return input.find(">") != string::npos;
}

string getOutputFile(const string& input, bool& append) {
    size_t pos = input.find(">>");
    if (pos != string::npos) {
        append = true;
        string filename = input.substr(pos + 2);
        size_t first = filename.find_first_not_of(" \t");
        return (first != string::npos) ? filename.substr(first) : "";
    }
    
    pos = input.find(">");
    if (pos != string::npos) {
        append = false;
        string filename = input.substr(pos + 1);
        size_t first = filename.find_first_not_of(" \t");
        return (first != string::npos) ? filename.substr(first) : "";
    }
    
    return "";
}

string getCleanCommand(const string& input) {
    size_t pos = input.find(">");
    if (pos != string::npos) {
        string clean = input.substr(0, pos);
        // trim trailing spaces
        size_t last = clean.find_last_not_of(" \t");
        return (last != string::npos) ? clean.substr(0, last + 1) : "";
    }
    return input;
}

int setupOutputRedirection(const string& filename, bool append) {
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
    if (fd == -1) {
        perror(filename.c_str());
        return -1;
    }
    
    int saved_stdout = dup(STDOUT_FILENO);
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return -1;
    }
    
    close(fd);
    return saved_stdout;
}

void restoreOutput(int saved_stdout) {
    if (saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
}

bool hasPipe(const string& input) {
    return input.find("|") != string::npos;
}

vector<string> splitPipe(const string& input) {
    vector<string> commands;
    stringstream ss(input);
    string cmd;
    
    while (getline(ss, cmd, '|')) {
        // trim spaces
        size_t first = cmd.find_first_not_of(" \t");
        size_t last = cmd.find_last_not_of(" \t");
        if (first != string::npos) {
            cmd = cmd.substr(first, last - first + 1);
            commands.push_back(cmd);
        }
    }
    return commands;
}

void executePipe(const vector<string>& commands) {
    int pipefd[2];
    int prev_fd = -1;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        if (i < commands.size() - 1) {
            pipe(pipefd);
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            
            if (i < commands.size() - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            
            // Parse command and arguments
            string cmd = commands[i];
            vector<string> args;
            stringstream ss(cmd);
            string token;
            while (ss >> token) {
                args.push_back(token);
            }
            
            // Convert to char* array for execvp
            vector<char*> argv;
            for (const string& arg : args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);
            
            // Execute command using execvp
            execvp(argv[0], argv.data());
        } else {
            // Parent process
            if (prev_fd != -1) close(prev_fd);
            if (i < commands.size() - 1) {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
            wait(NULL);
        }
    }
}
