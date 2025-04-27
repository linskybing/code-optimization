CC = g++
CFLAGS = -O3 -march=native

SRC = $(wildcard *.cpp)
TARGET = $(SRC:.cpp=)

all: $(TARGET)

popcount: popcount.cpp
	$(CC) $(CFLAGS) -msse4.2 $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET)