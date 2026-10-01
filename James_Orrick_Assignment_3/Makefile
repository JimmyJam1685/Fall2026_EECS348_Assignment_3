CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

TARGET = email_priority_queue
SRC = gemini_source.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
