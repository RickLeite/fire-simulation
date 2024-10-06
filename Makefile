CC = gcc
CFLAGS = -Wall -Wextra -pthread
SOURCES = main.c functions.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = fire_simulation
LOG_FILE = simulation_output.log

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(EXECUTABLE)
	./$(EXECUTABLE) | tee $(LOG_FILE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(LOG_FILE)

.PHONY: all run clean