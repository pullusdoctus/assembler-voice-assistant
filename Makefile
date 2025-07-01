# Makefile for Assembler Voice Assistant
CXX = g++
NASM = nasm

CXXFLAGS = -Wall -O2 -I./src `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lportaudio -lpocketsphinx -lstdc++ -lm -no-pie

SRC_DIR = src
OBJ_DIR = obj
BIN = voice_assistant

# Automatically find all source files
CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
ASM_SRCS = $(wildcard $(SRC_DIR)/*.asm)

# Generate object file names
CPP_OBJS = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
ASM_OBJS = $(ASM_SRCS:$(SRC_DIR)/%.asm=$(OBJ_DIR)/%.o)
ALL_OBJS = $(CPP_OBJS) $(ASM_OBJS)

# Default target
all: $(BIN)

# Create obj directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile C++ files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile assembly files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	$(NASM) -f elf64 $< -o $@

# Link everything into final executable
$(BIN): $(ALL_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

# Show what files will be compiled (for debugging)
show:
	@echo "C++ files: $(CPP_SRCS)"
	@echo "ASM files: $(ASM_SRCS)"
	@echo "C++ objects: $(CPP_OBJS)"
	@echo "ASM objects: $(ASM_OBJS)"
	@echo "All objects: $(ALL_OBJS)"

.PHONY: all clean show
