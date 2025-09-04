.PHONY: all run clean rebuild

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shell

SRCDIR = others_files_and_directories
SRCS = $(SRCDIR)/shell.cpp $(SRCDIR)/display_hostname.cpp $(SRCDIR)/echo.cpp $(SRCDIR)/ls.cpp $(SRCDIR)/pinfo.cpp $(SRCDIR)/search.cpp $(SRCDIR)/signals.cpp $(SRCDIR)/redirection.cpp $(SRCDIR)/system_commands.cpp $(SRCDIR)/pipe.cpp $(SRCDIR)/pipe_redirection.cpp $(SRCDIR)/autocomplete.cpp $(SRCDIR)/history.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lreadline

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/Header.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	gnome-terminal -- ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
