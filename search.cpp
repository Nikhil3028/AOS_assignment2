#include "Header.h"

// Search for target string in file
bool search_in_file(const string& file_path, const string& target) {
    FILE* file = fopen(file_path.c_str(), "r");
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

// Recursive search function
bool search_recursive(const string& current_path, const string& target) {
    DIR* dir = opendir(current_path.c_str());
    if (!dir) {
        return false;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip current directory (.) and parent directory (..)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        string entry_name = entry->d_name;
        string full_path = current_path + "/" + entry_name;
        
        // Check if the directory/file name matches the target
        if (entry_name == target) {
            closedir(dir);
            return true;
        }
        
        struct stat stat_buf;
        if (stat(full_path.c_str(), &stat_buf) == 0) {
            if (S_ISDIR(stat_buf.st_mode)) {
                // It's a directory - search recursively
                if (search_recursive(full_path, target)) {
                    closedir(dir);
                    return true;
                }
            }
        }
    }
    
    closedir(dir);
    return false;
}

// Search command implementation
void search(const string& filename) {
    string clean_filename = filename;
    
    // If no filename provided, try to read from stdin (for pipeline usage)
    if (clean_filename.empty()) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stdin)) {
            clean_filename = buffer;
            // Remove newline if present
            if (!clean_filename.empty() && clean_filename.back() == '\n') {
                clean_filename.pop_back();
            }
        }
        
        if (clean_filename.empty()) {
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
    
    bool found = search_recursive(string(cwd), clean_filename);
    
    cout << (found ? "True" : "False") << endl;
}
