CC = g++
CFLAGS = -O2
CXXFLAGS = $(CFLAGS)

BIN = gen main float-diff
all: $(BIN)

gen: gen.cpp
	$(CXX) $(CXXFLAGS) -mfma $< -o $@

float-diff: float-diff.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

main: main.cpp
	$(CC) $(CFLAGS) -mavx2 -mfma $< -o $@

.PHONY: clean

clean:
	rm -f $(BIN)
	rm -f *.bin