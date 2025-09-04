# POSIX Shell Implementation - Assignment A2

## Project Explanation

This project is a comprehensive implementation of a **POSIX shell** written in C++ that replicates the core functionality of Unix/Linux shells like bash. The shell provides an interactive command-line interface where users can execute both built-in commands and external system programs.


## File Structure

```
2025201081A2/
├── others/                     # Source code directory
│   ├── Header.h               # Main header file with all function declarations
│   ├── shell.cpp              # Main shell loop, command parsing, and tokenization
│   ├── display_hostname.cpp   # Prompt display with username@hostname:directory>
│   ├── echo.cpp               # Built-in echo command implementation
│   ├── ls.cpp                 # Built-in ls command with -a and -l flags
│   ├── pinfo.cpp              # Process information command (displays PID, status, memory)
│   ├── search.cpp             # Recursive file/folder search functionality
│   ├── signals.cpp            # Signal handling for CTRL-C, CTRL-Z, CTRL-D
│   ├── redirection.cpp        # I/O redirection implementation (<, >, >>)
│   ├── system_commands.cpp    # External command execution with fork/exec
│   ├── pipe.cpp               # Pipeline implementation for command chaining
│   ├── pipe_redirection.cpp   # Combined pipeline and redirection functionality
│   ├── autocomplete.cpp       # TAB completion for commands and files
│   └── history.cpp            # Command history with persistent storage
├── Makefile                   # Build configuration and compilation rules
└── README.md                  # This documentation file
```

## How to Run

### Prerequisites
- **Operating System**: Linux/Unix environment
- **Compiler**: GCC with C++17 support
- **Library**: GNU readline library for enhanced input handling

### Installation Steps

1. **Install Dependencies:**
   ```bash
   # Ubuntu/Debian systems
   sudo apt-get install libreadline-dev
   

2. **Navigate to Project Directory:**
   ```bash
   cd 2025201081A2
   ```

3. **Compile the Shell:**
   ```bash
   make
   ```

4. **Execute the Shell:**
   ```bash
   ./shell
   ```

### Build Commands
```bash
make           # Compile the shell
make run       # Compile and run in new terminal
make clean     # Remove object files and executable
make rebuild   # Clean and rebuild from scratch
```

##  Commands and Features Implementation

### **1. `echo` Command**
- **Purpose**: Display text to standard output
- **Implementation**: Custom string processing without `execvp()`
- **Features**: Supports quoted strings, escape sequences, and multiple arguments
- **Flow**: Parse arguments → Process escape sequences → Write to stdout
- **Usage**: `echo "Hello World"`, `echo $USER`

### **2. `pwd` Command**
- **Purpose**: Print current working directory
- **Implementation**: Uses `getcwd()` system call
- **Flow**: Get current directory → Display absolute path
- **Usage**: `pwd`

### **3. `cd` Command**
- **Purpose**: Change current directory
- **Implementation**: Uses `chdir()` system call
- **Features**: Supports `~` (home), `..` (parent), absolute and relative paths
- **Flow**: Parse path → Resolve special characters → Change directory → Update prompt
- **Usage**: `cd /home/user`, `cd ..`, `cd ~`

### **4. `ls` Command**
- **Purpose**: List directory contents
- **Implementation**: Uses `opendir()`, `readdir()`, `stat()` system calls
- **Features**: 
  - `-a` flag: Show hidden files
  - `-l` flag: Long format with permissions, size, date
- **Flow**: Parse flags → Open directory → Read entries → Format output → Display
- **Usage**: `ls`, `ls -l`, `ls -a`, `ls -la /etc`

### **5. `pinfo` Command**
- **Purpose**: Display process information
- **Implementation**: Reads from `/proc/[pid]/` filesystem
- **Features**: Shows PID, status, memory usage, executable path
- **Flow**: Get PID → Read /proc files → Parse information → Display formatted output
- **Usage**: `pinfo`, `pinfo 1234`

### **6. `search` Command**
- **Purpose**: Recursively search for files/folders
- **Implementation**: Uses `opendir()`, `readdir()` with recursive traversal
- **Features**: Returns "True" if found, "False" if not found
- **Flow**: Parse filename → Traverse directories recursively → Check matches → Return result
- **Usage**: `search filename.txt`, `echo "config.h" | search`

### **7. `history` Command**
- **Purpose**: Display command history
- **Implementation**: Maintains in-memory list with file persistence
- **Features**: 
  - Stores last 20 commands in `~/.shell_history`
  - `history` shows last 10 commands
  - `history <n>` shows last n commands
- **Flow**: Read history file → Display requested number of commands
- **Usage**: `history`, `history 5`

### **8. `clear` Command**
- **Purpose**: Clear terminal screen
- **Implementation**: Sends ANSI escape sequence
- **Flow**: Write clear screen sequence to stdout
- **Usage**: `clear`

### **9. `exit` Command**
- **Purpose**: Exit the shell
- **Implementation**: Clean shutdown with resource cleanup
- **Flow**: Save history → Close file descriptors → Exit gracefully
- **Usage**: `exit`

### **10. I/O Redirection**
- **Input Redirection (`<`)**: Redirects stdin from file
- **Output Redirection (`>`)**: Redirects stdout to file (overwrite)
- **Append Redirection (`>>`)**: Redirects stdout to file (append)
- **Implementation**: Parse redirection operators → Open files → Duplicate file descriptors → Execute command → Restore original descriptors
- **Usage**: `ls > output.txt`, `cat < input.txt`, `echo "text" >> file.txt`

### **11. Pipeline Implementation**
- **Purpose**: Chain commands where output of one becomes input of next
- **Implementation**: Creates pipes using `pipe()` system call, forks processes
- **Flow**: Parse pipe operators → Create pipes → Fork processes → Connect stdin/stdout → Execute commands → Wait for completion
- **Usage**: `ls | grep .cpp`, `ps aux | grep shell | wc -l`

### **12. Signal Handling**
- **CTRL-C (SIGINT)**: Interrupts foreground process
- **CTRL-Z (SIGTSTP)**: Sends process to background
- **CTRL-D (EOF)**: Graceful shell exit
- **Implementation**: Custom signal handlers using `sigaction()`

### **13. Background Processes**
- **Purpose**: Execute commands in background
- **Implementation**: Fork process without waiting
- **Flow**: Parse `&` operator → Fork → Execute in child → Display PID → Continue shell
- **Usage**: `sleep 10 &`, `./long_running_program &`

### **14. Command History Navigation**
- **UP Arrow**: Navigate to previous commands
- **DOWN Arrow**: Navigate to next commands
- **Implementation**: GNU readline library integration
- **Features**: Edit retrieved commands before execution

### **15. TAB Autocomplete**
- **Command Completion**: Built-in commands + system PATH commands
- **File Completion**: Current directory files with `/` suffix for directories
- **Implementation**: GNU readline with custom completion functions
- **Flow**: Detect TAB → Generate completions → Display matches → Fill common prefix

### **16. External System Commands**
- **Purpose**: Execute system programs and utilities
- **Implementation**: Fork/exec pattern with proper process management
- **Features**: Foreground execution with wait, background with `&`
- **Flow**: Parse command → Fork process → Execute with `execvp()` → Wait for completion
- **Usage**: `gcc file.c`, `ps aux`, `vim file.txt`

---

