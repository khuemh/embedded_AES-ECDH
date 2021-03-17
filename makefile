# Project directory tree map
# Project
# |-build
# |   |-*.o, *.exe 
# |-*.c, *.h

# Makefile rule
# target : prerequisites ...
# 	command 1
#	command 2

# $@: targets in rule --> $(@D): target dir
# $<: first prerequisites in rule
# $^: all prerequisites in rule

CXX := gcc
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD := ./build
TARGET := app 
INC := -I.
SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=$(BUILD)/%.o) # *.c --> *.o

all: build $(BUILD)/$(TARGET)
	@echo "Project Built Successfully"

$(BUILD)/%.o : %.c
	@echo "Compiling object..."
	$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $<

$(BUILD)/$(TARGET) : $(OBJ)
	@echo "Compiling taget..."
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -o $(BUILD)/$(TARGET) $(OBJ)

.PHONY: all build clean
build: 
	@echo "Create BUILD directory..."
	@mkdir -p $(BUILD)
clean:
	@echo "Clean BUILD directory..."
	-@rm -rvf $(BUILD)
