#include "Header.h"

// Minimal tokenize function for command separation
void tokenize(const string& input, string commands[], int& count) {
    count = 0;
    string cmd = "";
    
    for (char c : input) {
        if (c == ';') {
            if (!cmd.empty()) {
                commands[count++] = cmd;
                cmd = "";
            }
        } else if (c != ' ' || !cmd.empty()) {
            cmd += c;
        }
    }
    
    if (!cmd.empty()) {
        commands[count++] = cmd;
    }
}

// Main shell loop
int main() {
    string input;
    
    setup_signal_handlers();
    initialize_autocomplete();
    initializeHistory();

    while (true) {
        string prompt = display_hostname();
        
        char* line = readline(prompt.c_str());
        if (line == NULL) {
            handle_ctrl_d();
            break;
        }
        
        input = line;
        
        // Add to custom history if not empty
        if (!input.empty()) {
            addToHistory(input);
        }
        
        free(line);

        if (input.empty()) {
            continue;
        }

        // Tokenize input by semicolons
        string commands[100];
        int command_count;
        tokenize(input, commands, command_count);

    
        for (int i = 0; i < command_count; i++) {
            string current_input = commands[i];
            
            // Check if command contains both pipe and redirection
            if (hasPipeAndRedirection(current_input)) {
                executePipeWithRedirection(current_input);
                continue;
            }
            
            // Check if command contains pipe only
            if (hasPipe(current_input)) {
                executePipeline(current_input);
                continue;
            }
            
            // Check if command contains redirection only
            if (hasRedirection(current_input)) {
                executeSystemCommand(current_input);
                continue;
            }
            
            char buffer[1024];
            strcpy(buffer, current_input.c_str());

            char* token = strtok(buffer, " \t");
            if (!token) {
                continue;
            }

            string command = token;

            if (command == "exit") {
                // Make sure exit command is saved to history
                saveHistory();
                return 0;
            }
            else if (command == "history") {
                handleHistoryCommand(current_input);
            }
            else if (command == "echo") {
                echo(current_input);
            }
            else if (command == "pwd") {
                pwd(current_input);
            }
            else if (command == "clear") {
                clear_screen();
            }
            else if (command == "cd") {
                char* path = strtok(NULL, " \t");
                char* extra = strtok(NULL, " \t");
                if (extra) {
                    cout << "cd: too many arguments" << endl;
                } else {
                    cd(path);
                }
            }
            else if (command == "ls") {
                // Take everything after "ls"
                size_t pos = current_input.find("ls");
                string rest = (pos != string::npos) ? current_input.substr(pos + 2) : "";

                // Trim leading spaces
                size_t first = rest.find_first_not_of(" \t");
                if (first != string::npos) {
                    rest = rest.substr(first);
                }
                else {
                    rest = "";
                }

                ls(rest);
            }
            else if (command == "pinfo") {
                // Take everything after "pinfo"
                size_t pos = current_input.find("pinfo");
                string rest = (pos != string::npos) ? current_input.substr(pos + 5) : "";

                // Trim leading spaces
                size_t first = rest.find_first_not_of(" \t");
                if (first != string::npos) {
                    rest = rest.substr(first);
                }
                else {
                    rest = "";
                }

                pinfo(rest);
            }
            else if (command == "search") {
                char* filename = strtok(NULL, " \t");
                char* extra = strtok(NULL, " \t");
                if (extra) {
                    cout << "search: too many arguments" << endl;
                } else {
                    string filename_str = filename ? string(filename) : "";
                    search(filename_str);
                }
            }
            else {
                executeSystemCommand(current_input);
            }
        }
    }
    return 0;
}
