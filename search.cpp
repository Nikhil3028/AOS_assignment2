#include "Header.h"

bool searchRecursive(const string& currentPath, const string& target) {
    DIR* dir = opendir(currentPath.c_str());
    if (!dir) {
        return false;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip current directory (.) and parent directory (..)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        string entryName = entry->d_name;
        string fullPath = currentPath + "/" + entryName;
        
        // Check if this entry matches our target
        if (entryName == target) {
            closedir(dir);
            return true;
        }
        
        // If it's a directory, search recursively
        struct stat statbuf;
        if (stat(fullPath.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            if (searchRecursive(fullPath, target)) {
                closedir(dir);
                return true;
            }
        }
    }
    
    closedir(dir);
    return false;
}

void search(const string& filename) {
    int saved_stdout = -1;
    string cleanFilename = filename;
    
    if (hasRedirection(filename)) {
        bool append;
        string outputFile = getOutputFile(filename, append);
        if (!outputFile.empty()) {
            saved_stdout = setupOutputRedirection(outputFile, append);
            if (saved_stdout == -1) return;
            cleanFilename = getCleanCommand(filename);
        }
    }
    
    if (cleanFilename.empty()) {
        cout << "Usage: search <filename>" << endl;
        restoreOutput(saved_stdout);
        return;
    }
    
    // Get current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        cout << "Error getting current directory" << endl;
        return;
    }
    
    // Search recursively from current directory
    bool found = searchRecursive(string(cwd), cleanFilename);
    
    if (found) {
        cout << "True" << endl;
    } else {
        cout << "False" << endl;
    }
    
    restoreOutput(saved_stdout);
}
