#include "Header.h"

// Built-in commands for completion
static const char* builtin_commands[] = {
    "echo", "pwd", "cd", "ls", "pinfo", "search", "clear", "exit", "history", NULL
};

// Function to generate command completions
char* command_generator(const char* text, int state) {
    static int list_index, len;
    static bool checking_builtins;
    const char* name;
    
    if (!state) {
        list_index = 0;
        len = strlen(text);
        checking_builtins = true;
    }
    
    // Check built-in commands first
    if (checking_builtins) {
        while ((name = builtin_commands[list_index])) {
            list_index++;
            if (strncmp(name, text, len) == 0) {
                return strdup(name);
            }
        }
        // Finished with built-ins, switch to system commands
        checking_builtins = false;
        list_index = 0;
    }
    
    // Check system commands in PATH
    static bool path_searched = false;
    static vector<string> system_commands;
    
    if (!path_searched) {
        path_searched = true;
        
        // Get PATH environment variable
        char* path_env = getenv("PATH");
        if (path_env) {
            string path_str(path_env);
            stringstream ss(path_str);
            string path_dir;
            
            // Split PATH by ':'
            while (getline(ss, path_dir, ':')) {
                DIR* dir = opendir(path_dir.c_str());
                if (dir) {
                    struct dirent* entry;
                    while ((entry = readdir(dir)) != NULL) {
                        if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
                            string cmd_name(entry->d_name);
                            // Check if file is executable
                            string full_path = path_dir + "/" + cmd_name;
                            if (access(full_path.c_str(), X_OK) == 0) {
                                system_commands.push_back(cmd_name);
                            }
                        }
                    }
                    closedir(dir);
                }
            }
            
            // Remove duplicates and sort
            sort(system_commands.begin(), system_commands.end());
            system_commands.erase(unique(system_commands.begin(), system_commands.end()), 
                                system_commands.end());
        }
    }
    
    // Search through system commands
    while (list_index < (int)system_commands.size()) {
        const string& cmd = system_commands[list_index++];
        if (strncmp(cmd.c_str(), text, len) == 0) {
            return strdup(cmd.c_str());
        }
    }
    
    return NULL;
}

// Function to generate file/directory completions
char* filename_generator(const char* text, int state) {
    static DIR* dir = NULL;
    static int len;
    static char current_dir[PATH_MAX];
    struct dirent* entry;
    
    if (!state) {
        if (dir) {
            closedir(dir);
        }
        
        // Get current directory
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            return NULL;
        }
        
        dir = opendir(current_dir);
        if (!dir) {
            return NULL;
        }
        
        len = strlen(text);
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip all hidden files (files starting with '.')
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        if (strncmp(entry->d_name, text, len) == 0) {
            // Check if it's a directory and add '/' suffix
            struct stat file_stat;
            string full_path = string(current_dir) + "/" + entry->d_name;
            if (stat(full_path.c_str(), &file_stat) == 0 && S_ISDIR(file_stat.st_mode)) {
                string result = string(entry->d_name) + "/";
                return strdup(result.c_str());
            } else {
                return strdup(entry->d_name);
            }
        }
    }
    
    if (dir) {
        closedir(dir);
        dir = NULL;
    }
    
    return NULL;
}

// Main completion function
char** shell_completion(const char* text, int start, int end __attribute__((unused))) {
    char** matches = NULL;
    
    // Get the current line to analyze context
    const char* line = rl_line_buffer;
    
    // Check if we're completing the first word (command)
    // Look for any non-whitespace characters before current position
    bool is_command = true;
    for (int i = 0; i < start; i++) {
        if (line[i] != ' ' && line[i] != '\t') {
            is_command = false;
            break;
        }
    }
    
    // If we're at position 0 or only whitespace before, it's a command
    // Otherwise, it's a filename/argument
    if (is_command && start == 0) {
        // At the very beginning - could be command or filename
        // Try both command and filename completion
        matches = rl_completion_matches(text, command_generator);
        if (!matches) {
            matches = rl_completion_matches(text, filename_generator);
        }
    } else if (is_command) {
        // After whitespace - complete commands
        matches = rl_completion_matches(text, command_generator);
    } else {
        // After a command - complete filenames
        matches = rl_completion_matches(text, filename_generator);
    }
    
    return matches;
}

// Function to initialize autocomplete
void initialize_autocomplete() {
    // Set our completion function
    rl_attempted_completion_function = shell_completion;
    
    // Set characters that break words for completion
    rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";
    
    // Don't append space after completion if it's a directory
    rl_completion_append_character = '\0';
}

// Function to display multiple matches
void display_matches(char** matches, int num_matches, int max_length) {
    cout << endl;
    
    // Calculate number of columns based on terminal width
    int term_width = 80; // Default width
    char* columns_env = getenv("COLUMNS");
    if (columns_env) {
        term_width = atoi(columns_env);
    }
    
    int cols = term_width / (max_length + 2);
    if (cols < 1) cols = 1;
    
    // Display matches in columns
    for (int i = 1; i <= num_matches; i++) {
        cout << left << setw(max_length + 2) << matches[i];
        if (i % cols == 0 || i == num_matches) {
            cout << endl;
        }
    }
    
    // Redisplay the prompt and current line
    rl_forced_update_display();
}
