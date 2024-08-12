# STM8 device (for supported devices see stm8s.h)
ifeq ($(origin DEVICE), undefined)
    $(error "DEVICE not defined ! define it in your application makefile")
endif

# Defines the compiler for stm8s.h. Could be done in my application code instead
# Flags for the sdcc compiler. -lstm8 and -mstm8 let it know its compiling for an stm8 target
COMPILER := SDCC
CFLAGS := -mstm8 -lstm8 -D$(DEVICE) -D$(COMPILER)
#specify compiler
CC:=sdcc
#specify library formatter
AR:=sdar
#specify device
DEVICE:=STM8S007

#build dir for .rel files
BUILD_DIR:=build
#output dir
OUTPUT_DIR:=
#Name of this library:
TARGET:=$(OUTPUT_DIR)spl.lib

#Source file (.c) directory for lib
LIB_SOURCE_DIR:=./src
# define SPL_SOURCE_PREFIX from the target dependent .mk file
#This allows us to only build the .c files related to our MCU
include ./mk/$(DEVICE).mk
#find all source files
LIB_SOURCE := $(foreach prefix, $(SPL_SOURCE_PREFIX), $(wildcard $(LIB_SOURCE_DIR)/$(prefix)*.c))
#string sub to object file targets
LIB_OBJECTS := $(LIB_SOURCE:$(LIB_SOURCE_DIR)/%.c=./$(BUILD_DIR)/%.rel)

#.h file inc dirs
LIB_INC_DIR:=./inc
PRJ_INC_DIR:=../../inc
# Add paths for the preprocessor to search for the .h files to #include
INCLUDES := -I$(LIB_INC_DIR) -I$(PRJ_INC_DIR)

# The vpathdirective is used to specify where source files can live, to allow make to find them
# https://makefiletutorial.com/#the-vpath-directive
VPATH:=$(LIB_SOURCE_DIR)

# This prevents make from not running clean if for some reason we had a file named clean
# https://makefiletutorial.com/#phony
.PHONY: clean


all: $(BUILD_DIR) $(TARGET)

# Below is a pattern rule to make all the required object files (.rel) from their .c files.
# Any targets that match ./$(BUILD_DIR)/%.rel are built from their %.c match (where % is the stem match)
# As the path for the .c files is not specified in the prereq, it needs to be on the VPATH
# so that make can find them.
# -o flag sets the dir to put all build files in
./$(BUILD_DIR)/%.rel : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $? -o ./$(BUILD_DIR)/

# The rule to make the desired library
$(TARGET) : $(LIB_OBJECTS)
	$(AR) -rc $@ $^

# Make rule to create the build directory if it doesnt exist
$(BUILD_DIR) :
	mkdir $(BUILD_DIR)

# Rule to delete all the build files
clean:
	rm -r $(BUILD_DIR)
	rm $(TARGET)
