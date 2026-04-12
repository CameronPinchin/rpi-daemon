CC = gcc 
CFLAGS = -g -Wall 

TARGET = server

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) src/main.c -o bin/$(TARGET)

clean:
	rm -rf *.o bin/$(TARGET)
