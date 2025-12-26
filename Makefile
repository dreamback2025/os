CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = multi_process_scheduler
SOURCES = main.c memory_manager.c process_manager.c scheduler.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run