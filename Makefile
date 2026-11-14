CC = gcc
CFLAGS = -std=c89 -Wall -Wextra -pedantic -Werror
CPPFLAGS = -D_POSIX_C_SOURCE=199309L

BUILD_DIR = build
EXECUTABLE = treasure-hunt

BASE_SOURCES = src/main.c src/game.c src/map.c src/linkedlist.c \
               src/random.c src/terminal.c src/newSleep.c
SOURCES = $(BASE_SOURCES)

ifeq ($(DARK),1)
CPPFLAGS += -DDARK
SOURCES += src/darkmode.c
endif

OBJECTS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean normal dark test

all: $(EXECUTABLE)

normal:
	$(MAKE) clean
	$(MAKE) DARK=0 all

dark:
	$(MAKE) clean
	$(MAKE) DARK=1 all

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -c $< -o $@

test: | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc tests/test_game.c src/game.c src/map.c \
	    src/linkedlist.c src/random.c src/terminal.c src/newSleep.c \
	    -o $(BUILD_DIR)/test_game
	./$(BUILD_DIR)/test_game

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)
