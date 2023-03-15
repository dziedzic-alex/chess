CXX = g++
BIN = chess
SRCS = $(wildcard *.cpp)
HDS = $(wildcard *.hpp)

.PHONY: all
all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) -o $@ $^

$(SRCS): $(HDS)

.PHONY: clean
clean:
	rm -f $(BIN)

