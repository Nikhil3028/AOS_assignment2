#include "Header.h"

// Print ls -l permissions
void print_permissions(mode_t mode) {
    cout << (S_ISDIR(mode) ? 'd' : (S_ISLNK(mode) ? 'l' : '-'));
    cout << ((mode & S_IRUSR) ? 'r' : '-');
    cout << ((mode & S_IWUSR) ? 'w' : '-');
    cout << ((mode & S_IXUSR) ? 'x' : '-');
    cout << ((mode & S_IRGRP) ? 'r' : '-');
    cout << ((mode & S_IWGRP) ? 'w' : '-');
    cout << ((mode & S_IXGRP) ? 'x' : '-');
    cout << ((mode & S_IROTH) ? 'r' : '-');
    cout << ((mode & S_IWOTH) ? 'w' : '-');
    cout << ((mode & S_IXOTH) ? 'x' : '-');
}

// Print a single directory with flags
void print_dir(const string& dir_path, bool show_all, bool long_format) {
    DIR* dir = opendir(dir_path.c_str());
    if (!dir) {
        perror(dir_path.c_str());
        return;
    }
    vector<string> files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        string file_name = entry->d_name;
        if (!show_all && file_name[0] == '.') {
            continue; // Skip hidden if not -a
        }
        files.push_back(file_name);
    }
    closedir(dir);

    sort(files.begin(), files.end());

    for (const string& file_name : files) {
        struct stat st;
        if (stat(file_name.c_str(), &st) != 0) {
            continue;
        }

        if (long_format) {
            print_permissions(st.st_mode);
            cout << ' ' << setw(2) << st.st_nlink << ' ';
            struct passwd* pw = getpwuid(st.st_uid);
            struct group* gr = getgrgid(st.st_gid);
            cout << (pw ? pw->pw_name : "") << ' ';
            cout << (gr ? gr->gr_name : "") << ' ';
            cout << setw(6) << st.st_size << ' ';
            char timebuf[80];
            struct tm* mtm = localtime(&st.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", mtm);
            cout << timebuf << ' ';
            if (S_ISDIR(st.st_mode)) {
                cout << "\033[31m" << file_name << "\033[0m";
            }
            else {
                cout << file_name;
            }
            cout << endl;
        } else {
            // Check if stdout is a pipe (not a terminal)
            if (!isatty(STDOUT_FILENO)) {
                // Output one file per line for pipes
                cout << file_name << endl;
            } else {
                // Normal terminal output with spaces
                if (S_ISDIR(st.st_mode)) {
                    cout << "\033[31m" << file_name << "\033[0m ";
                }
                else {
                    cout << file_name << "  ";
                }
            }
        }
    }
    if (!long_format && isatty(STDOUT_FILENO)) {
        cout << endl;
    }
}


void ls(const string& arg_line) {
    int saved_stdout = -1;
    string clean_arg_line = arg_line;
    
    if (hasRedirection(arg_line)) {
        bool append;
        string filename = getOutputFile(arg_line, append);
        if (!filename.empty()) {
            saved_stdout = setupOutputRedirection(filename, append);
            if (saved_stdout == -1) {
                return;
            }
            clean_arg_line = getCleanCommand(arg_line);
        }
    }
    
    vector<string> tokens;
    stringstream ss(clean_arg_line);
    string tok;
    while (ss >> tok) {
        tokens.push_back(tok);
    }

    bool show_all = false, long_format = false;
    vector<string> dirs;

    // Parse flags and dirs
    for (const string& t : tokens) {
        if (!t.empty() && t[0] == '-') {
            if (t.length() == 1) {
                cout << "ls: invalid option -- '-'" << endl;
                return;
            }
            for (size_t i = 1; i < t.length(); ++i) {
                if (t[i] == 'a') {
                    show_all = true;
                }
                else if (t[i] == 'l') {
                    long_format = true;
                }
                else {
                    cout << "ls: invalid option -- '" << t[i] << "'" << endl;
                    return;
                }
            }
        } else {
            dirs.push_back(t);
        }
    }
    
    // Check for too many directory arguments
    if (dirs.size() > 1) {
        cout << "ls: too many arguments" << endl;
        return;
    }
    
    if (dirs.empty()) {
        dirs.push_back(".");
    }

    // Save current working directory
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }
    string saved_dir = cwd;

    // Process directory argument
    for (const string& dir : dirs) {
        if (dir != ".") {
            // Check if directory exists before trying to access it
            struct stat stat_buf;
            if (stat(dir.c_str(), &stat_buf) != 0 || !S_ISDIR(stat_buf.st_mode)) {
                perror(dir.c_str());
                continue;
            }

            // Go into directory using your cd()
            cd((char*)dir.c_str());
            print_dir(".", show_all, long_format);
            // Restore working directory
            cd((char*)saved_dir.c_str());
        } else {
            print_dir(".", show_all, long_format);
        }
    }
    
    restoreOutput(saved_stdout);
}
