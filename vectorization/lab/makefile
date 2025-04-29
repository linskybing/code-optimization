CC = g++
CFLAGS = -O2
CXXFLAGS = $(CFLAGS)

BIN = gen main float-diff
all: $(BIN)

gen: gen.cpp
	$(CXX) -O3 $< -o $@

float-diff: float-diff.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

main: main.cpp
	$(CC) $(CFLAGS) -mavx2 $< -o $@

.PHONY: clean

clean:
	rm -f $(BIN)
	rm -f *.bin