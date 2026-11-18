CC := gcc

CC_FLAGS := -I./src -I. $(shell pkg-config --cflags sdl2)
LD_FLAGS := $(shell pkg-config --libs sdl2)


SRC_DIR := src
BUILD_DIR := src/build

C_SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
C_OBJS := $(C_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

MAIN := $(BUILD_DIR)/main

.PHONY: all clean run

all: $(MAIN)

$(MAIN): $(C_OBJS)
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@ $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

run: $(MAIN)
	@$(MAIN)