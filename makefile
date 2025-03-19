CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
TARGET = deadlock
OBJFILES = deadlock.c

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(TARGET)