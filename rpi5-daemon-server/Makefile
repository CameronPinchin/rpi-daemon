CC = gcc
CFLAGS = -Wall -pthread
CFLAGS_DEBUG = -g -Wall -pthread -DDEBUG

C_FILES = src/main.c src/controller.c

TARGET = server
TARGET_DEBUG = server_debug

all: $(TARGET)

debug: $(TARGET_DEBUG)

$(TARGET):
	$(CC) $(CFLAGS) $(C_FILES) -o bin/$(TARGET)

$(TARGET_DEBUG):
	$(CC) $(CFLAGS_DEBUG) $(C_FILES) -o bin/$(TARGET_DEBUG)

clean:
	rm -rf *.o bin/$(TARGET)
