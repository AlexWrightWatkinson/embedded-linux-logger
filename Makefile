CC := gcc

CFLAGS_COMMON := -Wall -Wextra -Werror -Iinclude
CFLAGS_DEBUG := -O0 -g
CFLAGS_RELEASE := -O2 -DNDEBUG

TARGET := edlogd
SRC := \
		src/main.c \
		src/sensor.c \
		src/ring_buffer.c \
		src/logger_binary.c \
		src/logger_journald.c

OBJ := $(SRC:.c=.o)

BUILD ?= release

ifeq ($(BUILD),debug)
    CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
else
    CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

