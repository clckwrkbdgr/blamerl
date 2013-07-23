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

run: deps $(BIN)
	$(TERMINAL) './$(BIN)'

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) -c $<

.PHONY: deps clean

deps: $(OBJ:.o=.cpp)
	$(CXX) -MM $^ > $@

clean:
	$(RM) -rf *.o $(BIN) deps

include deps
