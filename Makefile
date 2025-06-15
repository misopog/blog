CC = tcc
CFLAGS = -g -Wall -Wextra -std=c99 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lcmark-gfm -lcmark-gfm-extensions
TARGET = blog

SRC = main.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean run cleanrun

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf output/

run:
	./$(TARGET)

cleanrun: clean all run

