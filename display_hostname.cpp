#include "Header.h"

string display_hostname() {
    static bool firstRun = true;      // track first execution
    static string launchDir;          // store directory where program started

    char hostname[1024];
    char cwd[PATH_MAX];
    char* username = getenv("USER");

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return "$ ";
    }

    if (firstRun) {
        // Capture launch directory on first run
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            return "$ ";
        }
        launchDir = cwd;

        // First prompt → only hostname
        string prompt = "\033[34m";
        prompt += (username ? username : "user");
        prompt += "@" + string(hostname) + ":--> \033[0m";
        firstRun = false;
        return prompt;
    }

    // Get current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return "$ ";
    }

    string cwdStr = cwd;
    const char* home = getenv("HOME");

    // Replace /home/<user> with ~
    if (home && cwdStr.find(home) == 0) {
        cwdStr.replace(0, string(home).length(), "~");
    }

    string prompt = "\033[34m";
    prompt += (username ? username : "user");
    prompt += "@" + string(hostname);

    // Case 1: if in launch directory → only hostname
    if (string(cwd) == launchDir) {
        prompt += ":--> \033[0m";
        return prompt;
    }

    // Case 2: otherwise → username@host:cwd
    prompt += ":";

    // cwd: blue if home (~), yellow otherwise
    if (!cwdStr.empty() && cwdStr[0] == '~') {
        prompt += "\033[34m" + cwdStr;
    } else {
        prompt += "\033[33m" + cwdStr;
    }

    prompt += "\033[34m$ \033[0m";
    return prompt;
}
