#include "Header.h"

// Prints ls -l permissions
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

// Prints a single directory with flags
void print_dir(const string& dirPath, bool showAll, bool longFormat) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        perror(dirPath.c_str());
        return;
    }
    vector<string> files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        string fname = entry->d_name;
        if (!showAll && fname[0] == '.') continue; // skip hidden if not -a
        files.push_back(fname);
    }
    closedir(dir);

    sort(files.begin(), files.end());

    for (const string& fname : files) {
        struct stat st;
        if (stat(fname.c_str(), &st) != 0) continue;

        if (longFormat) {
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
            if (S_ISDIR(st.st_mode))
                cout << "\033[31m" << fname << "\033[0m";
            else
                cout << fname;
            cout << endl;
        } else {
            if (S_ISDIR(st.st_mode))
                cout << "\033[31m" << fname << "\033[0m ";
            else
                cout << fname << " ";
        }
    }
    if (!longFormat) cout << endl;
}

// Main ls dispatcher: pass argument string after "ls" (e.g., "-la dir1 dir2")
void ls(const string& argline) {
    vector<string> tokens;
    stringstream ss(argline);
    string tok;
    while (ss >> tok) tokens.push_back(tok);

    bool showAll = false, longFormat = false;
    vector<string> dirs;

    // Parse flags and dirs
    for (const string& t : tokens) {
        if (!t.empty() && t[0] == '-') {
            for (size_t i = 1; i < t.length(); ++i) {
                if (t[i] == 'a') showAll = true;
                else if (t[i] == 'l') longFormat = true;
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
    
    if (dirs.empty()) dirs.push_back(".");

    // Save current working directory
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }
    string savedDir = cwd;

    // Loop through each dir argument
    for (size_t d = 0; d < dirs.size(); ++d) {
        if (dirs[d] != ".") {
            // Check if directory exists before trying to access it
            struct stat statbuf;
            if (stat(dirs[d].c_str(), &statbuf) != 0 || !S_ISDIR(statbuf.st_mode)) {
                perror(dirs[d].c_str());
                continue;
            }

            // go into directory using your cd()
            cd((char*)dirs[d].c_str());

            if (dirs.size() > 1) cout << dirs[d] << ":" << endl;
            print_dir(".", showAll, longFormat);

            // restore working directory
            cd((char*)savedDir.c_str());
        } else {
            if (dirs.size() > 1) cout << dirs[d] << ":" << endl;
            print_dir(".", showAll, longFormat);
        }

        if (dirs.size() > 1 && d + 1 < dirs.size())
            cout << endl;
    }
}
