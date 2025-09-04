#include "Header.h"

// Process information command implementation
void pinfo(const string& pid_str) {
    string clean_pid_str = pid_str;
       
    pid_t pid;
    if (clean_pid_str.empty()) {
        pid = getpid();
    } else {
        try {
            pid = stoi(clean_pid_str);
        } catch (const exception&) {
            cout << "Invalid PID: " << clean_pid_str << endl;
            return;
        }
    }
    
    // Read process status from /proc/[pid]/stat
    string stat_path = "/proc/" + to_string(pid) + "/stat";
    FILE* stat_file = fopen(stat_path.c_str(), "r");
    
    if (!stat_file) {
        cout << "Process with PID " << pid << " not found" << endl;
        return;
    }
    
    char buffer[1024];
    string line;
    if (fgets(buffer, sizeof(buffer), stat_file) != NULL) {
        line = buffer;
    }
    fclose(stat_file);
    
    // Parse the stat file
    vector<string> tokens;
    
    // Split by spaces, but handle the command name which can contain spaces and is in parentheses
    bool in_parentheses = false;
    string current_token = "";
    
    for (char c : line) {
        if (c == '(') {
            in_parentheses = true;
            current_token += c;
        } else if (c == ')') {
            in_parentheses = false;
            current_token += c;
        } else if (c == ' ' && !in_parentheses) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token = "";
            }
        } else {
            current_token += c;
        }
    }
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    if (tokens.size() < 23) {
        cout << "Error reading process information" << endl;
        return;
    }
    
    // Extract information
    char status = tokens[2][0]; // Process state
    string vm_size = tokens[22]; // Virtual memory size in pages
    
    // Convert VM size from pages to KB (assuming 4KB pages)
    long vm_size_kb = stol(vm_size) * 4;
    
    // Check if process is in foreground
    pid_t pgrp = stol(tokens[4]); // Process group ID
    int tty_nr = stol(tokens[6]); // TTY number
    
    // Check if process is in foreground by comparing process group with terminal foreground process group
    bool is_foreground = false;
    if (tty_nr > 0) {
        // Get the foreground process group of the terminal
        pid_t fg_pgrp = tcgetpgrp(STDIN_FILENO);
        if (fg_pgrp == pgrp) {
            is_foreground = true;
        }
    }
    
    // Get executable path from /proc/[pid]/exe
    string exe_path = "/proc/" + to_string(pid) + "/exe";
    char executable_path[PATH_MAX];
    ssize_t len = readlink(exe_path.c_str(), executable_path, sizeof(executable_path) - 1);
    
    string exec_path;
    if (len != -1) {
        executable_path[len] = '\0';
        exec_path = string(executable_path);
    } else {
        exec_path = "N/A";
    }
    
    // Format and display the output
    cout << "Process Status -- ";
    switch (status) {
        case 'R':
            cout << (is_foreground ? "R+" : "R");
            break;
        case 'S':
            cout << (is_foreground ? "S+" : "S");
            break;
        case 'Z':
            cout << "Z";
            break;
        case 'T':
            cout << "T";
            break;
        default:
            cout << status << (is_foreground ? "+" : "");
            break;
    }
    cout << endl;
    
    cout << "memory -- " << vm_size_kb << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exec_path << endl;
    
   
}
