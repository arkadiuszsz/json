SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
PREFIX=/usr/local
BIN=json

histo: $(SRC)
	$(CC) $^ -lm -I deps -o $(BIN)

install: $(BIN)
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(BIN)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: clean install uninstall
