# My attempt at writing a makefile fully from scratch.
# Inspiration from:
# https://makefiletutorial.com/
# https://github.com/bschwand/STM8-SPL-SDCC/blob/master/Project/STM8S_StdPeriph_Template/SDCC/Makefile
# https://github.com/EMBEDONIX/stm8/blob/master/examples/leddance/Makefile


# Specifies the compiler I am using
# The directory to put all output files in
# The processor we use
# The target filename of the exccecutable we are making
CC := sdcc
BUILD_DIR := build
PROCESSOR := stm8s007c8
TARGET := ./$(BUILD_DIR)/flight_software.ihx

# The family of our microcontroller, for stm8s.h. Could be done in my application code instead
# Defines the compiler for stm8s.h. Could be done in my application code instead
# Flags for the sdcc compiler. -lstm8 and -mstm8 let it know its compiling for an stm8 target
DEVICE := STM8S007
COMPILER := __CSMC__
CFLAGS := -mstm8 -lstm8 -D$(DEVICE) -D$(COMPILER)

# TODO: Uncomment the one you are using
OS := windows
#OS := linux

# Sets the variable RM_CMD to the linux or windows command accordingly
ifeq ($(OS), linux)
	RM_CMD = rm -r
else ifeq ($(OS), windows)
	RM_CMD = rmdir /Q /S
endif

# Set project folder and files 
PRJ_SRC_DIR := ./src
PRJ_INC_DIR := ./inc
PRJ_SOURCE := $(wildcard $(PRJ_SRC_DIR)/*.c)

# Below performs string substitution on the PRJ_SOURCE to change from e.g. ./src/main.c to ./build/main.rel
# See https://makefiletutorial.com/#string-substitution for details
PRJ_OBJECTS := $(PRJ_SOURCE:$(PRJ_SRC_DIR)/%.c=./$(BUILD_DIR)/%.rel)

# Set SPL folder and files
SPL_INC_DIR := ./libs/STM8S_StdPeriph_Driver/inc
SPL_LIB_DIR := ./libs
SPL_LIB := projectlib.lib

# Add paths for the preprocessor to search for the .h files to #include
INCLUDES := -I$(PRJ_INC_DIR) -I$(SPL_INC_DIR)

# The vpathdirective is used to specify where source files can live, to allow make to find them
# https://makefiletutorial.com/#the-vpath-directive
VPATH=$(PRJ_SRC_DIR)

# This prevents make from not running clean if for some reason we had a file named clean
# https://makefiletutorial.com/#phony
.PHONY: clean

# The default make target, as it is listed first
all: $(BUILD_DIR) $(TARGET)

# Make rule to create the build directory if it doesnt exist
$(BUILD_DIR) :
	mkdir $(BUILD_DIR)

# Below is a pattern rule to make all the required object files (.rel) from their .c files.
# Any targets that match ./$(BUILD_DIR)/%.rel are built from their %.c match (where % is the stem match)
# As the path for the .c files is not specified in the prereq, it needs to be on the VPATH
# so that make can find them.
# -o flag sets the dir to put all build files in
./$(BUILD_DIR)/%.rel : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $? -o ./$(BUILD_DIR)/

# The rule to make the desired binary excecutable
$(TARGET) : $(PRJ_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SPL_LIB) -L $(SPL_LIB_DIR) $^ -o $(TARGET)

# Rule to delete all the build files
clean:
	$(RM_CMD) $(BUILD_DIR)







