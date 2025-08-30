#include "Header.h"

void pinfo(const string& pidStr) {
    pid_t pid;
    
    if (pidStr.empty()) {
        // If no PID provided, use current process PID
        pid = getpid();
    } else {
        // Convert string to PID
        try {
            pid = stoi(pidStr);
        } catch (const exception& e) {
            cout << "Invalid PID: " << pidStr << endl;
            return;
        }
    }
    
    // Read process status from /proc/[pid]/stat
    string statPath = "/proc/" + to_string(pid) + "/stat";
    ifstream statFile(statPath);
    
    if (!statFile.is_open()) {
        cout << "Process with PID " << pid << " not found" << endl;
        return;
    }
    
    string line;
    getline(statFile, line);
    statFile.close();
    
    // Parse the stat file
    istringstream iss(line);
    string token;
    vector<string> tokens;
    
    // Split by spaces, but handle the command name which can contain spaces and is in parentheses
    bool inParentheses = false;
    string currentToken = "";
    
    for (char c : line) {
        if (c == '(') {
            inParentheses = true;
            currentToken += c;
        } else if (c == ')') {
            inParentheses = false;
            currentToken += c;
        } else if (c == ' ' && !inParentheses) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken = "";
            }
        } else {
            currentToken += c;
        }
    }
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    if (tokens.size() < 23) {
        cout << "Error reading process information" << endl;
        return;
    }
    
    // Extract information
    char status = tokens[2][0]; // Process state
    string vmSize = tokens[22]; // Virtual memory size in pages
    
    // Convert VM size from pages to KB (assuming 4KB pages)
    long vmSizeKB = stol(vmSize) * 4;
    
    // Check if process is in foreground
    string statmPath = "/proc/" + to_string(pid) + "/stat";
    pid_t pgrp = stol(tokens[4]); // Process group ID
    int tty_nr = stol(tokens[6]); // TTY number
    
    // Check if process is in foreground by comparing process group with terminal foreground process group
    bool isForeground = false;
    if (tty_nr > 0) {
        // Get the foreground process group of the terminal
        pid_t fg_pgrp = tcgetpgrp(STDIN_FILENO);
        if (fg_pgrp == pgrp) {
            isForeground = true;
        }
    }
    
    // Get executable path from /proc/[pid]/exe
    string exePath = "/proc/" + to_string(pid) + "/exe";
    char executablePath[PATH_MAX];
    ssize_t len = readlink(exePath.c_str(), executablePath, sizeof(executablePath) - 1);
    
    string execPath;
    if (len != -1) {
        executablePath[len] = '\0';
        execPath = string(executablePath);
    } else {
        execPath = "N/A";
    }
    
    // Format and display the output
    cout << "Process Status -- ";
    switch (status) {
        case 'R':
            cout << (isForeground ? "R+" : "R");
            break;
        case 'S':
            cout << (isForeground ? "S+" : "S");
            break;
        case 'Z':
            cout << "Z";
            break;
        case 'T':
            cout << "T";
            break;
        default:
            cout << status << (isForeground ? "+" : "");
            break;
    }
    cout << endl;
    
    cout << "memory -- " << vmSizeKB << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << execPath << endl;
}
