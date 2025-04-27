CC = g++
CFLAGS = -O0 -Wall -march=native

SRC = $(wildcard *.cpp)
TARGET = $(SRC:.cpp=)

all: $(TARGET)

%: %.cpp
	$(CC) $(CFLAGS) -mavx2 $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)