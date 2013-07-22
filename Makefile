APP = blamerl
LIBS = -lncurses
MODULES = main.o game.o console.o
TEST_RUN = xfce4-terminal -e

run: $(APP)
	$(TEST_RUN) './$^'

$(APP): $(MODULES)
	$(CXX) -o $@ $(LIBS) $^

game.o: game.h
main.o: game.h console.h
console.o: console.h

clean:
	$(RM) -rf *.o $(APP)
