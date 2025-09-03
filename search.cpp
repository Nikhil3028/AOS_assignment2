#include "Header.h"

bool searchInFile(const string& filePath, const string& target) {
    FILE* file = fopen(filePath.c_str(), "r");
    if (!file) {
        return false;
    }
    
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        string line(buffer);
        if (line.find(target) != string::npos) {
            fclose(file);
            return true;
        }
    }
    
    fclose(file);
    return false;
}

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
        
        // Check if the directory/file name matches the target
        if (entryName == target) {
            closedir(dir);
            return true;
        }
        
        struct stat statbuf;
        if (stat(fullPath.c_str(), &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                // It's a directory - search recursively
                if (searchRecursive(fullPath, target)) {
                    closedir(dir);
                    return true;
                }
            }
        }
    }
    
    closedir(dir);
    return false;
}

void search(const string& filename) {
    string cleanFilename = filename;
    
    // If no filename provided, try to read from stdin (for pipeline usage)
    if (cleanFilename.empty()) {
        string line;
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stdin)) {
            line = buffer;
            // Remove newline if present
            if (!line.empty() && line.back() == '\n') {
                line.pop_back();
            }
            cleanFilename = line;
        }
        
        if (cleanFilename.empty()) {
            cout << "Usage: search <filename>" << endl;
            return;
        }
    }
    
    // Get current working directory and search recursively
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        cout << "Error getting current directory" << endl;
        return;
    }
    
    bool found = searchRecursive(string(cwd), cleanFilename);
    
    cout << (found ? "True" : "False") << endl;
}
