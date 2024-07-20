# My attempt at writing a makefile fully from scratch. Works on windows

# Specifies the compiler I am using
# The directory to put all output files in
# The processor we use
# The target filename of the exccecutable we are making
CC := sdcc
BUILD_DIR := build
PROCESSOR := stm8s007c8
TARGET := ./$(BUILD_DIR)/flight_software.ihx

# Flags for the sdcc compiler. -lstm8 and -mstm8 let it know its compiling for an stm8 target
# The family of our microcontroller
CFLAGS := -mstm8 -lstm8
DEVICE := STM8S007

# TODO: Uncomment the one you are using
OS := windows
#OS := linux

# Sets the variable RM_CMD to the linux or windows command accordingly
ifeq ($(OS), linux)
	RM_CMD = rm -r
else ifeq ($(OS), windows)
	RM_CMD = rmdir
endif

# Set project folder and files 
PRJ_SRC_DIR := ./src
PRJ_INC_DIR := ./inc
PRJ_SOURCE := $(wildcard $(PRJ_SRC_DIR)/*.c)
PRJ_OBJECTS := $(addprefix ./$(BUILD_DIR)/, $(PRJ_SOURCE:.c=.rel))

# Set SPL folder and files
SPL_SRC_DIR := ./

INCLUDES := -I$(PRJ_INC_DIR) -I

VPATH=$(PRJ_SRC_DIR)

# Below is a pattern rule to make all the required object files (.rel) from their .c files.
# Any targets that match ./$(BUILD_DIR)/%.rel are built from their %.c match (where % is the stem match)
# As the path for the .c files is not specified in the prereq, it needs to be on the VPATH
# so that make can find them
./$(BUILD_DIR)/%.rel : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -D$(DEVICE) $(INCLUDES) - $< -o $@

# all: $(BUILD_DIR) $(TARGET)

$(TARGET) : $(PRJ_OBJECTS)

# $(BUILD_DIR) :
# 	mkdir $(BUILD_DIR)

# clean:
# 	$(RM_CMD) $(BUILD_DIR)







