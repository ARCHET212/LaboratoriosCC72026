# Makefile simple para parking.c
CC = gcc
CFLAGS = -Wall -pthread -lrt
TARGET = parking
SRC = parking.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run