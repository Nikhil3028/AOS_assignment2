#include "Header.h"

string display_hostname() {
    static bool firstRun = true;
    static string launchDir;

    char hostname[1024];
    char cwd[PATH_MAX];
    char* username = getenv("USER");

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return ":-->";
    }

    // Get current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return ":-->";
    }

    string cwdStr = cwd;
    
    // Capture launch directory on first run
    if (firstRun) {
        launchDir = cwdStr;
        firstRun = false;
    }

    // Build base prompt with bold green username@hostname
    string prompt = "\001\033[1;32m\002";  // Bold green
    prompt += (username ? username : "user");
    prompt += "@" + string(hostname);
    prompt += "\001\033[0m\002";  // Reset formatting

    // If in launch directory → only hostname
    if (cwdStr == launchDir) {
        prompt += "\001\033[1;37m\002:--> \001\033[0m\002";  // Bold white arrow
        return prompt;
    }

    // Otherwise → username@host:cwd
    prompt += "\001\033[1;37m\002:\001\033[0m\002";  // Bold white colon
    
    // Add colored directory path
    if (cwdStr[0] == '~') {
        prompt += "\001\033[36m\002" + cwdStr;  // cyan for home
    } else {
        prompt += "\001\033[33m\002" + cwdStr;  // yellow for others
    }

    prompt += "\001\033[1;37m\002--> \001\033[0m\002";  // Bold white arrow
    return prompt;
}
