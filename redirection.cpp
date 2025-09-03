#include "Header.h"

// Check if input contains redirection operators
bool hasRedirection(const string& input) {
    return input.find(">") != string::npos || input.find("<") != string::npos;
}

// Get output file from redirection
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

// Get input file from redirection
string getInputFile(const string& input) {
    size_t pos = input.find("<");
    if (pos != string::npos) {
        string filename = input.substr(pos + 1);
        size_t first = filename.find_first_not_of(" \t");
        if (first != string::npos) {
            size_t end = filename.find_first_of(" \t>", first);
            return (end != string::npos) ? filename.substr(first, end - first) : filename.substr(first);
        }
    }
    return "";
}

// Get clean command without redirection operators
string getCleanCommand(const string& input) {
    string clean = input;
    
    // Remove output redirection
    size_t pos = clean.find(">");
    if (pos != string::npos) {
        clean = clean.substr(0, pos);
    }
    
    // Remove input redirection
    pos = clean.find("<");
    if (pos != string::npos) {
        clean = clean.substr(0, pos);
    }
    
    size_t last = clean.find_last_not_of(" \t");
    return (last != string::npos) ? clean.substr(0, last + 1) : "";
}

// Setup output redirection
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

// Setup input redirection
int setupInputRedirection(const string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        perror(filename.c_str());
        return -1;
    }
    
    int saved_stdin = dup(STDIN_FILENO);
    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return -1;
    }
    
    close(fd);
    return saved_stdin;
}

// Restore input redirection
void restoreInput(int saved_stdin) {
    if (saved_stdin != -1) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
}

// Restore output redirection
void restoreOutput(int saved_stdout) {
    if (saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
}
