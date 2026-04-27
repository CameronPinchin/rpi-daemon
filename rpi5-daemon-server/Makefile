CC = gcc
CC_CROSSCOMP = aarch64-linux-gnu-gcc
CFLAGS = -Wall -pthread
CFLAGS_DEBUG = -g -Wall -pthread -DDEBUG

C_FILES = src/main.c src/controller.c

TARGET = server
TARGET_DEBUG = server_debug
TARGET_CROSSCOMPILED = server_arm64
TARGET_DEBUG_CROSSCOMPILED = server_arm64_debug

all: $(TARGET)

debug: $(TARGET_DEBUG)

cross: $(TARGET_CROSSCOMPILED)

cross-debug: $(TARGET_DEBUG_CROSSCOMPILED)

$(TARGET):
	$(CC) $(CFLAGS) $(C_FILES) -o bin/$(TARGET)

$(TARGET_CROSSCOMPILED):
	$(CC_CROSSCOMP) $(CFLAGS) $(C_FILES) -o bin/$(TARGET_CROSSCOMPILED)

$(TARGET_DEBUG):
	$(CC) $(CFLAGS_DEBUG) $(C_FILES) -o bin/$(TARGET_DEBUG)

$(TARGET_DEBUG_CROSSCOMPILED):
	$(CC_CROSSCOMP) $(CFLAGS_DEBUG) $(C_FILES) -o bin/$(TARGET_DEBUG_CROSSCOMPILED)

clean:
	rm -rf *.o bin/*
