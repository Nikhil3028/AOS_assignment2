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
#include <cstdio>
#include <exception>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

// SHELL DISPLAY AND PROMPT FUNCTIONS
string display_hostname();
void clear_screen();

// BUILT-IN COMMAND FUNCTIONS
void echo(const string &input);
void pwd(const string& input = "");
void cd(char* path);
void ls(const string& argline);
void print_dir(const string& dirPath, bool showAll, bool longFormat);
void print_permissions(mode_t mode);
void pinfo(const string& pidStr);
void search(const string& filename);

// SYSTEM COMMAND EXECUTION FUNCTION
void executeSystemCommand(const string& input);

// SIGNAL HANDLING FUNCTION
void setup_signal_handlers();
void set_foreground_process(pid_t pid);
void handle_ctrl_d();

// I/O REDIRECTION FUNCTION
bool hasRedirection(const string& input);
string getOutputFile(const string& input, bool& append);
string getInputFile(const string& input);
string getCleanCommand(const string& input);
int setupOutputRedirection(const string& filename, bool append);
int setupInputRedirection(const string& filename);
void restoreOutput(int saved_stdout);
void restoreInput(int saved_stdin);

// PIPELINE FUNCTION
bool hasPipe(const string& input);
void executePipeline(const string& input);
void executePipe(const vector<string>& commands);
bool isInternalCommand(const string& command);
vector<string> parseCommand(const string& cmdStr);
vector<string> splitPipe(const string& input);
void executeInternalCommand(const string& cmdStr, int inputFd, int outputFd);
void executeExternalCommand(const string& cmdStr, int inputFd, int outputFd);

// PIPELINE WITH REDIRECTION FUNCTION
bool hasPipeAndRedirection(const string& input);
bool pipeHasInputRedirection(const vector<string>& commands);
bool pipeHasOutputRedirection(const vector<string>& commands);
string getPipeInputFile(const vector<string>& commands);
string getPipeOutputFile(const vector<string>& commands, bool& append);
vector<string> getCleanPipeCommands(const vector<string>& commands);
void executePipeWithRedirection(const string& input);

// AUTOCOMPLETE FUNCTION
char* command_generator(const char* text, int state);
char* filename_generator(const char* text, int state);
char** shell_completion(const char* text, int start, int end);
void initialize_autocomplete();
void display_matches(char** matches, int num_matches, int max_length);

// HISTORY FUNCTION
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
