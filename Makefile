CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = password_generator

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.c functions.c functions.h
	$(CC) $(CFLAGS) main.c functions.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
