CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -O2
SRCDIR  = src
BINDIR  = bin
TARGET  = $(BINDIR)/lexer

SRCS = $(SRCDIR)/main.c $(SRCDIR)/lexer.c $(SRCDIR)/symtable.c
OBJS = $(SRCS:.c=.o)

.PHONY: all run test clean

all: $(BINDIR) $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET) $(ARGS)

test: all
	bash tests/run_tests.sh

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
	rm -rf $(BINDIR)
