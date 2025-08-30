#include "Header.h"

void display_hostname() {
    static bool firstRun = true;      // track first execution
    static string launchDir;          // store directory where program started

    char hostname[1024];
    char cwd[PATH_MAX];
    char* username = getenv("USER");

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return;
    }

    if (firstRun) {
        // Capture launch directory on first run
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            return;
        }
        launchDir = cwd;

        // First prompt → only hostname
        cout << "\033[34m"
             << (username ? username : "user")
             << "@" << hostname
             << ":--> \033[0m";
        firstRun = false;
        return;
    }

    // Get current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return;
    }

    string cwdStr = cwd;
    const char* home = getenv("HOME");

    // Replace /home/<user> with ~
    if (home && cwdStr.find(home) == 0) {
        cwdStr.replace(0, string(home).length(), "~");
    }

    // Case 1: if in launch directory → only hostname
    if (cwd == launchDir) {
        cout << "\033[34m"
             << (username ? username : "user")
             << "@" << hostname
             << ":--> \033[0m";
        return;
    }

    // Case 2: otherwise → username@host:cwd
    cout << "\033[34m"
         << (username ? username : "user")
         << "@" << hostname
         << ":";

    // cwd: blue if home (~), yellow otherwise
    if (!cwdStr.empty() && cwdStr[0] == '~') {
        cout << "\033[34m" << cwdStr;
    } else {
        cout << "\033[33m" << cwdStr;
    }

    cout << "\033[34m$ \033[0m";
}
