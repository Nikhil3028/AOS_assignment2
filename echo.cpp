#include "Header.h"

void echo(const string &input) {
    int saved_stdout = -1;
    string cleanInput = input;
    
    if (hasRedirection(input)) {
        bool append;
        string filename = getOutputFile(input, append);
        if (!filename.empty()) {
            saved_stdout = setupOutputRedirection(filename, append);
            if (saved_stdout == -1) return;
            cleanInput = getCleanCommand(input);
        }
    }
    
    string text = cleanInput.substr(4); // Remove "echo"
    
    // trim leading spaces
    size_t first = text.find_first_not_of(" \t");
    if (first != string::npos) text = text.substr(first);
    
    cout << text << endl;
    
    restoreOutput(saved_stdout);
}


void pwd(const string& input) {
    int saved_stdout = -1;
    
    if (hasRedirection(input)) {
        bool append;
        string filename = getOutputFile(input, append);
        if (!filename.empty()) {
            saved_stdout = setupOutputRedirection(filename, append);
            if (saved_stdout == -1) return;
        }
    }
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << cwd << endl;
    } else {
        perror("pwd");
    }
    
    restoreOutput(saved_stdout);
}

void clear_screen() {
    cout << "\033[2J\033[H";
}

void cd(char* path) {
    static string oldpwd;
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) { perror("getcwd"); return; }

    if (!path || !strcmp(path, "~")) {
        if (chdir(getenv("HOME"))) perror("cd");
    } 
    else if (!strcmp(path, "-")) {
        if (oldpwd.empty()) cerr << "cd: OLDPWD not set\n";
        else if (chdir(oldpwd.c_str())) perror("cd");
        else cout << oldpwd << '\n';
    } 
    else if (chdir(path)) {
        const char* home = getenv("HOME");
        if (home) {
            string full = string(home) + "/" + path;
            if (chdir(full.c_str())) {
                if (access(path, F_OK) && access(full.c_str(), F_OK))
                    cout << "cd: " << path << " No such file or directory\n";
                else if (access(path, X_OK))
                    cout << "cd: " << path << "Permission denied\n";
                else perror("cd");
            }
        }
    }
    oldpwd = cwd;
}
