CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shell

SRCS = shell.cpp display_hostname.cpp echo.cpp ls.cpp pinfo.cpp search.cpp signals.cpp redirection.cpp system_commands.cpp pipe.cpp pipe_redirection.cpp autocomplete.cpp history.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

shell: shell.o display_hostname.o echo.o ls.o pinfo.o search.o signals.o redirection.o system_commands.o pipe.o pipe_redirection.o autocomplete.o history.o
	g++ -Wall -Wextra -std=c++17 -o shell shell.o display_hostname.o echo.o ls.o pinfo.o search.o signals.o redirection.o system_commands.o pipe.o pipe_redirection.o autocomplete.o history.o -lreadline

%.o: %.cpp Header.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	gnome-terminal -- ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
