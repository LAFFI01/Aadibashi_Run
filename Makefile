# ══════════════════════════════════════════════════════
# Caveman Escape — Makefile
# ══════════════════════════════════════════════════════

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 \
          -O2 -Iinclude
LDFLAGS = -lm
TARGET  = bin/caveman_escape
SRCDIR  = src
SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/physics.c \
          $(SRCDIR)/display.c \
          $(SRCDIR)/enemy_ai.c \
          $(SRCDIR)/records.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

run: all
	./$(TARGET)
