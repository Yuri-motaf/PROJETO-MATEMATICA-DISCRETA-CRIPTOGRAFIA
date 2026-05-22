CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = src/main.c src/desencriptacao.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = rsa_desencriptacao

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean run
