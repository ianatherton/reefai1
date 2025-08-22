CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = reef
SRCS = $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)

install-deps:
	sudo apt update
	sudo apt install -y build-essential libraylib-dev

.PHONY: all clean install-deps
