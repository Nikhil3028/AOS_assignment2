CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shell

SRCS = shell.cpp display_hostname.cpp echo.cpp ls.cpp pinfo.cpp search.cpp signals.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp Header.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	gnome-terminal -- ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
