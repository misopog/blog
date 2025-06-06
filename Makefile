CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Ilib/md4c/src
TARGET = blog

SRC = main.c \
	lib/md4c/src/md4c.c \
	lib/md4c/src/md4c-html.c \
	lib/md4c/src/entity.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run:
	./$(TARGET)

cleanrun: clean all run
