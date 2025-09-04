#ifndef PROMPT_H
#define PROMPT_H


#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <limits.h>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <exception>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

string display_hostname();
void echo(const string &input);
void pwd(const string& input = "");
void cd(char* path);
void ls(const string& argline);
void print_dir(const string& dirPath, bool showAll, bool longFormat);
void print_permissions(mode_t mode);
void pinfo(const string& pidStr);
void search(const string& filename);
void clear_screen();
void setup_signal_handlers();
void set_foreground_process(pid_t pid);
void handle_ctrl_d();

// Redirection functions
bool hasRedirection(const string& input);
string getOutputFile(const string& input, bool& append);
string getInputFile(const string& input);
string getCleanCommand(const string& input);
int setupOutputRedirection(const string& filename, bool append);
int setupInputRedirection(const string& filename);
void restoreOutput(int saved_stdout);
void restoreInput(int saved_stdin);


// System command functions
void executeSystemCommand(const string& input);

// Pipeline functions
bool hasPipe(const string& input);
void executePipeline(const string& input);
void executePipe(const vector<string>& commands);
bool isInternalCommand(const string& command);
vector<string> parseCommand(const string& cmdStr);
vector<string> splitPipe(const string& input);
void executeInternalCommand(const string& cmdStr, int inputFd, int outputFd);
void executeExternalCommand(const string& cmdStr, int inputFd, int outputFd);

// Pipeline with redirection functions
bool hasPipeAndRedirection(const string& input);
bool pipeHasInputRedirection(const vector<string>& commands);
bool pipeHasOutputRedirection(const vector<string>& commands);
string getPipeInputFile(const vector<string>& commands);
string getPipeOutputFile(const vector<string>& commands, bool& append);
vector<string> getCleanPipeCommands(const vector<string>& commands);
void executePipeWithRedirection(const string& input);

// Autocomplete functions
char* command_generator(const char* text, int state);
char* filename_generator(const char* text, int state);
char** shell_completion(const char* text, int start, int end);
void initialize_autocomplete();
void display_matches(char** matches, int num_matches, int max_length);

// History functions
void initializeHistory();
void addToHistory(const string& command);
void handleHistoryCommand(const string& input);
void displayHistory(int num);
string getPreviousCommand();
string getNextCommand();
void resetHistoryNavigation();
bool isHistoryCommand(const string& input);
void loadHistory();
void saveHistory();
string getHistoryFilePath();

#endif
