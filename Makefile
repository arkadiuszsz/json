# Install
BIN = json

# Compiler
CC = gcc
DCC = clang

#Flags
DFLAGS = -g -Wall -Wextra -Werror -Wformat -Wunreachable-code
DFLAGS += -fstack-protector -Winline
CFLAGS = -O3 -fno-gcse

.PHONY: release
release: $(BIN)

.PHONY: debug
debug: CFLAGS = $(DFLAGS)
debug: $(BIN)

# Objects
SRCS = src/json.c example/test.c
OBJS = $(SRCS: .c = .o)

# Build
$(BIN): $(SRCS)
	@mkdir -p bin
	$(CC) $^ $(CFLAGS) -o $@
	@mv -f $(BIN) bin/

# Misc
clean:
	rm -f bin/$(BIN) $(OBJS)

all:
	release

.PHONY: clean all
