#include "Header.h"

#define MAX_HISTORY 20
#define HISTORY_FILE ".shell_history"

static vector<string> command_history;
static int history_index = -1;
static string current_input_buffer = "";

// Get the history file path in home directory
string getHistoryFilePath() {
    const char* home = getenv("HOME");
    if (!home) {
        home = getpwuid(getuid())->pw_dir;
    }
    return string(home) + "/" + HISTORY_FILE;
}

// Load history from file
void loadHistory() {
    string historyFile = getHistoryFilePath();
    ifstream file(historyFile);
    string line;
    
    command_history.clear();
    
    while (getline(file, line)) {
        if (!line.empty()) {
            command_history.push_back(line);
        }
    }
    file.close();
    
    // Load into readline history as well
    clear_history();
    for (const string& cmd : command_history) {
        add_history(cmd.c_str());
    }
}

// Save history to file
void saveHistory() {
    string historyFile = getHistoryFilePath();
    ofstream file(historyFile);
    
    for (const string& cmd : command_history) {
        file << cmd << endl;
    }
    file.close();
}

// Add command to history
void addToHistory(const string& command) {
    if (command.empty()) {
        return;
    }
    
    // Don't add duplicate consecutive commands
    if (!command_history.empty() && command_history.back() == command) {
        return;
    }
    
    command_history.push_back(command);
    
    // Maintain maximum of 20 commands
    if (command_history.size() > MAX_HISTORY) {
        command_history.erase(command_history.begin());
    }
    
    // Add to readline history
    add_history(command.c_str());
    
    // Save to file
    saveHistory();
    
    // Reset history navigation index
    history_index = -1;
}

// Display history command
void displayHistory(int num = 10) {
    int start = 0;
    int total = command_history.size();
    
    if (num > 0 && num < total) {
        start = total - num;
    }
    
    for (int i = start; i < total; i++) {
        cout << command_history[i] << endl;
    }
}

// Handle history command
void handleHistoryCommand(const string& input) {
    // Parse the command
    istringstream iss(input);
    string cmd;
    iss >> cmd; // "history"
    
    string numStr;
    if (iss >> numStr) {
        // history <num>
        try {
            int num = stoi(numStr);
            if (num <= 0) {
                cout << "history: invalid number" << endl;
                return;
            }
            displayHistory(num);
        } catch (const exception& e) {
            cout << "history: invalid number" << endl;
        }
    } else {
        // history (default 10)
        displayHistory();
    }
}

// Initialize history system
void initializeHistory() {
    loadHistory();
    
    // Set up readline history settings
    using_history();
    stifle_history(MAX_HISTORY);
    
    // Reset navigation state
    history_index = -1;
    current_input_buffer = "";
}

// Get previous command for UP arrow
string getPreviousCommand() {
    if (command_history.empty()) {
        return "";
    }
    
    if (history_index == -1) {
        // First time pressing UP, start from the end
        history_index = command_history.size() - 1;
        return command_history[history_index];
    } else if (history_index > 0) {
        // Go to previous command
        history_index--;
        return command_history[history_index];
    }
    
    // Already at the first command, stay there
    return command_history[history_index];
}

// Get next command for DOWN arrow
string getNextCommand() {
    if (command_history.empty() || history_index == -1) {
        return "";
    }
    
    if (history_index < (int)command_history.size() - 1) {
        // Go to next command
        history_index++;
        return command_history[history_index];
    } else {
        // At the most recent command, reset to empty input
        history_index = -1;
        return "";
    }
}

// Reset history navigation
void resetHistoryNavigation() {
    history_index = -1;
    current_input_buffer = "";
}

// Check if command is history command
bool isHistoryCommand(const string& input) {
    return input.substr(0, 7) == "history" && 
           (input.length() == 7 || input[7] == ' ');
}
