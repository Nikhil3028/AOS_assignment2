#include "Header.h"

void echo(const string &input) {
    string data = (input.size() > 4) ? input.substr(4) : "";
    string result;
    bool space = false;

    for (char ch : data) {
        if (ch == ';') break;                    // stop at ';'
        if (isspace((unsigned char)ch)) {
            space = true;
        } else {
            if (space && !result.empty()) result += ' ';
            result += ch;
            space = false;
        }
    }
    cout << result << '\n';
}


void pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << cwd << endl;
    } else {
        perror("pwd");
    }
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
