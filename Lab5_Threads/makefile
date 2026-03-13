CC = gcc
NUM_THREADS ?= 8
CFLAGS = -Wall -Wextra -pthread -O2 -DNUM_THREADS=$(NUM_THREADS)
TARGET = analyzer
OBJS = main_linux.o log_processor.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main_linux.o: main_linux.c log_processor.h
	$(CC) $(CFLAGS) -c main_linux.c

log_processor.o: log_processor.c log_processor.h
	$(CC) $(CFLAGS) -c log_processor.c

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Para comparar rendimiento con diferente número de hilos
run2:
	$(MAKE) clean
	$(MAKE) NUM_THREADS=2
	./$(TARGET)

run4:
	$(MAKE) clean
	$(MAKE) NUM_THREADS=4
	./$(TARGET)

run8:
	$(MAKE) clean
	$(MAKE) NUM_THREADS=8
	./$(TARGET)

.PHONY: clean run run2 run4 run8