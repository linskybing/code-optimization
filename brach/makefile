CC = g++
CFLAGS = -O0 -Wall -mavx2 -fopenmp

SRC = $(wildcard *.cpp)
TARGET = $(SRC:.cpp=)

all: $(TARGET)

%: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)