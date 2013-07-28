XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal -e
else
	TERMINAL = screen
endif

BIN = blamerl
LIBS = -lncurses
SOURCES = $(wildcard *.cpp)
OBJ = $(SOURCES:.cpp=.o)
CXXFLAGS = -Werror -Wall

run: deps $(BIN)
	$(TERMINAL) './$(BIN)'

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: deps clean Makefile

deps: $(OBJ:.o=.cpp)
	$(CXX) -MM $^ > $@

clean:
	$(RM) -rf *.o $(BIN) deps

include deps
