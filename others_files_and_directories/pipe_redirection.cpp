#include "Header.h"

// Check if pipeline has any redirection
bool hasPipeAndRedirection(const string& input) {
    return hasPipe(input) && hasRedirection(input);
}

// Check if pipe command has input redirection in first command
bool pipeHasInputRedirection(const vector<string>& commands) {
    if (!commands.empty()) {
        const string& firstCmd = commands.front();
        return firstCmd.find("<") != string::npos;
    }
    return false;
}

// Check if pipe command has output redirection in final command
bool pipeHasOutputRedirection(const vector<string>& commands) {
    if (!commands.empty()) {
        const string& lastCmd = commands.back();
        return lastCmd.find(">") != string::npos;
    }
    return false;
}

// Get input file from first pipe command
string getPipeInputFile(const vector<string>& commands) {
    if (!commands.empty()) {
        const string& firstCmd = commands.front();
        return getInputFile(firstCmd);
    }
    return "";
}

// Get output file from final pipe command
string getPipeOutputFile(const vector<string>& commands, bool& append) {
    if (!commands.empty()) {
        const string& lastCmd = commands.back();
        return getOutputFile(lastCmd, append);
    }
    return "";
}

// Get clean pipe commands (remove redirection from first and last commands)
vector<string> getCleanPipeCommands(const vector<string>& commands) {
    vector<string> cleanCommands = commands;
    if (!cleanCommands.empty()) {
        // Clean first command (remove input redirection)
        string& firstCmd = cleanCommands.front();
        firstCmd = getCleanCommand(firstCmd);
        
        // Clean last command (remove output redirection)
        string& lastCmd = cleanCommands.back();
        lastCmd = getCleanCommand(lastCmd);
    }
    return cleanCommands;
}

// Enhanced pipe execution with input/output redirection support
void executePipeWithRedirection(const string& input) {
    vector<string> commands = splitPipe(input);
    if (commands.empty()) return;
    
    bool hasInputRedir = pipeHasInputRedirection(commands);
    bool hasOutputRedir = pipeHasOutputRedirection(commands);
    
    string inputFile = "";
    string outputFile = "";
    bool append = false;
    int saved_stdin = -1;
    int saved_stdout = -1;
    
    // Handle input redirection for first command
    if (hasInputRedir) {
        inputFile = getPipeInputFile(commands);
        if (!inputFile.empty()) {
            saved_stdin = setupInputRedirection(inputFile);
            if (saved_stdin == -1) return;
        }
    }
    
    // Handle output redirection for last command
    if (hasOutputRedir) {
        outputFile = getPipeOutputFile(commands, append);
        if (!outputFile.empty()) {
            saved_stdout = setupOutputRedirection(outputFile, append);
            if (saved_stdout == -1) {
                if (saved_stdin != -1) restoreInput(saved_stdin);
                return;
            }
        }
    }
    
    // Execute pipe with clean commands
    vector<string> cleanCommands = getCleanPipeCommands(commands);
    executePipe(cleanCommands);
    
    // Restore redirections
    if (saved_stdin != -1) {
        restoreInput(saved_stdin);
    }
    if (saved_stdout != -1) {
        restoreOutput(saved_stdout);
    }
}
