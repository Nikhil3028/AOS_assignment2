#include "Header.h"

// Echo command implementation
void echo(const string &input) {
    string text = input.substr(4); // Remove "echo"
    
    // Trim leading spaces
    size_t first = text.find_first_not_of(" \t");
    if (first != string::npos) {
        text = text.substr(first);
    }
    
    cout << text << endl;
    cout.flush();
}

// Print working directory command
void pwd(const string& ) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << cwd << endl;
    } else {
        perror("pwd");
    }
}

// Clear screen function
void clear_screen() {
    cout << "\033[2J\033[H";
}

// Change directory command implementation
void cd(char* path) {
    static string prev_dir;     
    char cwd[PATH_MAX];

    // Save current working directory before changing
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }
    string current_dir = cwd;
    string target;

    // Case 1: no argument → go to home
    if (!path || strlen(path) == 0) {
        char* home = getenv("HOME");
        if (!home) {
            cerr << "cd: HOME not set\n";
            return;
        }
        target = home;
    }
    // Case 2: cd -
    else if (strcmp(path, "-") == 0) {
        if (prev_dir.empty()) {
            cerr << "cd: OLDPWD not set\n";
            return;
        }
        target = prev_dir;
        cout << prev_dir << "\n";   // Print the path like bash
    }
    // Case 3: cd ~ or cd ~/subdir
    else if (path[0] == '~') {
        char* home = getenv("HOME");
        if (!home) {
            cerr << "cd: HOME not set\n";
            return;
        }
        target = string(home) + string(path + 1);
    }
    // Case 4: cd . or cd ..
    else if (strcmp(path, ".") == 0) {
        target = current_dir;   // Stay in same place
    }
    else if (strcmp(path, "..") == 0) {
        target = current_dir + "/..";
    }
    // Case 5: normal path
    else {
        target = path;
    }

    // Attempt to change directory
    if (chdir(target.c_str()) != 0) {
        if (errno == EACCES) {
            cerr << "cd: permission denied: " << target << "\n";
        }
        else if (errno == ENOENT) {
            cerr << "cd: no such file or directory: " << target << "\n";
        }
        else {
            perror("cd");
        }
        return;
    }

    // Update prev_dir only on success
    prev_dir = current_dir;
}