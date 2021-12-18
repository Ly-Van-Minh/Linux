############################ Descriptions ###########################
#Author: Ly Van Minh
#University: Hanoi University of Science and Technology
#Date: 09/30/2021
#####################################################################

vpath %.h inc
vpath %.c src
RED 		= \033[1;31m
GREEN 		= \033[1;32m
YELLOW 		= \033[1;33m
BLUE 		= \033[1;34m
RESET		= \033[1;0m
INC_DIR 	= inc
SRC_DIR 	= src
BUILD_DIR 	= build
CC 			= gcc
CFLAGS 		= -g -Wall -Werror -I$(INC_DIR)
RM 			= rm -rf
SRCS 		= $(wildcard */*.c)
OBJS 		= $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(notdir $(SRCS))))
TARGET 		= OTA.elf
#####################################################################

all: $(BUILD_DIR)/$(TARGET) $(OBJS)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "$(YELLOW)Linking ...$(RESET)"
	$(CC) $(CFLAGS) -o $@ $^
	@echo Finished!
	@echo Target: $(TARGET) contained in directory $(CURDIR)/build

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@echo "$(GREEN)Compiling ...$(RESET)"
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	@echo "$(RED)Removing ... $(RESET)"
	$(RM) $(BUILD_DIR)/*

#####################################################################
