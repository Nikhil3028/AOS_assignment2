#include "Header.h"

// Check if pipe command has redirection in final command
bool pipeHasRedirection(const vector<string>& commands) {
    if (!commands.empty()) {
        const string& lastCmd = commands.back();
        return lastCmd.find(">") != string::npos;
    }
    return false;
}

// Get redirection info from final pipe command
string getPipeOutputFile(const vector<string>& commands, bool& append) {
    if (!commands.empty()) {
        const string& lastCmd = commands.back();
        return getOutputFile(lastCmd, append);
    }
    return "";
}

// Get clean pipe commands (remove redirection from final command)
vector<string> getCleanPipeCommands(const vector<string>& commands) {
    vector<string> cleanCommands = commands;
    if (!cleanCommands.empty()) {
        string& lastCmd = cleanCommands.back();
        lastCmd = getCleanCommand(lastCmd);
    }
    return cleanCommands;
}

// Execute pipe with redirection support
void executePipeWithRedirection(const vector<string>& commands) {
    bool hasRedir = pipeHasRedirection(commands);
    bool append = false;
    string outputFile = "";
    int saved_stdout = -1;
    
    if (hasRedir) {
        outputFile = getPipeOutputFile(commands, append);
        if (!outputFile.empty()) {
            saved_stdout = setupOutputRedirection(outputFile, append);
            if (saved_stdout == -1) return;
        }
    }
    
    // Execute pipe with clean commands
    vector<string> cleanCommands = getCleanPipeCommands(commands);
    executePipe(cleanCommands);
    
    // Restore output if redirected
    if (saved_stdout != -1) {
        restoreOutput(saved_stdout);
    }
}
