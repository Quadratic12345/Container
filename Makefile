CC = gcc

CFLAGS = -Wall -Wextra -Werror -D_GNU_SOURCE \
         -Iinclude

TARGET = container

SRC = \
    src/main.c \
    src/container.c \
    src/namespace.c \
    src/filesystem.c \
    src/cgroup.c \
    src/process.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
