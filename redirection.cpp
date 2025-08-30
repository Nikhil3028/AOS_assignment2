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
