#include "Header.h"

// Minimal tokenize function
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

int main() {
    string input;
    
    setup_signal_handlers();

    while (true) {
        string prompt = display_hostname();
        
        char* line = readline(prompt.c_str());
        if (line == NULL) {
            handle_ctrl_d();
            break;
        }
        
        input = line;
        
        // Add to history if not empty
        if (!input.empty()) {
            add_history(line);
        }
        
        free(line);

        if (input.empty()) continue;

        // Tokenize input by semicolons
        string commands[100];
        int commandCount;
        tokenize(input, commands, commandCount);

    
        for (int i = 0; i < commandCount; i++) {
            string currentInput = commands[i];
            
            // Check if command contains pipe
            if (hasPipe(currentInput)) {
                executePipeline(currentInput);
                continue;
            }
            
            char buffer[1024];
            strcpy(buffer, currentInput.c_str());

            char* token = strtok(buffer, " \t");
            if (!token) continue;

            string command = token;

            if (command == "exit") {
                return 0;
            }
            else if (command == "echo") {
                echo(currentInput);
            }
            else if (command == "pwd") {
                pwd(currentInput);
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
                // take everything after "ls"
                size_t pos = currentInput.find("ls");
                string rest = (pos != string::npos) ? currentInput.substr(pos + 2) : "";

                // trim leading spaces
                size_t first = rest.find_first_not_of(" \t");
                if (first != string::npos) rest = rest.substr(first);
                else rest = "";

                ls(rest);
            }
            else if (command == "pinfo") {
                // take everything after "pinfo"
                size_t pos = currentInput.find("pinfo");
                string rest = (pos != string::npos) ? currentInput.substr(pos + 5) : "";

                // trim leading spaces
                size_t first = rest.find_first_not_of(" \t");
                if (first != string::npos) rest = rest.substr(first);
                else rest = "";

                pinfo(rest);
            }
            else if (command == "search") {
                char* filename = strtok(NULL, " \t");
                char* extra = strtok(NULL, " \t");
                if (extra) {
                    cout << "search: too many arguments" << endl;
                } else {
                    string filenameStr = filename ? string(filename) : "";
                    search(filenameStr);
                }
            }
            else {
                executeSystemCommand(currentInput);
            }
        }
    }
    return 0;
}
