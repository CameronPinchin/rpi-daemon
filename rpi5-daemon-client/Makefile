CC = gcc
CFLAGS = -g -Wall
CFLAGS_DEBUG = -g -Wall -DDEBUG

TARGET = client
TARGET_DEBUG = client_debug

all: $(TARGET)

debug: $(TARGET_DEBUG)

$(TARGET):
	$(CC) $(CFLAGS) src/main.c src/parser.c -o bin/$(TARGET)

$(TARGET_DEBUG):
	$(CC) $(CFLAGS_DEBUG) src/main.c src/parser.c -o bin/$(TARGET_DEBUG)

clean:
	rm -rf *.o bin/$(TARGET)
