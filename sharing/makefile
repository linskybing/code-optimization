CC = g++
CFLAGS = -O0 -Wall -fopenmp

SRC = $(wildcard *.cpp)
TARGET = $(SRC:.cpp=)

all: $(TARGET)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)