# Makefile para Assembler Voice Assistant con SDL2

CXX = g++
NASM = nasm
CXXFLAGS = -Wall -O2 -I./src
#LDFLAGS = -lpocketsphinx -lsphinxbase -lstdc++ -lm -lSDL2 -lSDL2_ttf -lSDL2_image
LDFLAGS = -lsphinxad -lsphinxbase -lpocketsphinx -lstdc++ -lm -lSDL2 -lSDL2_ttf -lSDL2_image -no-pie

SRC_DIR = src
OBJ_DIR = obj
BIN = voice_assistant
UI_BIN = ui_demo

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/audio.cpp
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/audio.o $(OBJ_DIR)/assembler.o

UI_SRC = $(SRC_DIR)/ui.cpp
UI_OBJ = $(OBJ_DIR)/ui.o

all: $(BIN) $(UI_BIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/audio.o: $(SRC_DIR)/audio.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/assembler.o: $(SRC_DIR)/assembler.asm | $(OBJ_DIR)
	$(NASM) -f elf64 $< -o $@

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/ui.o: $(SRC_DIR)/ui.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(UI_BIN): $(OBJ_DIR)/ui.o $(OBJ_DIR)/assembler.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lSDL2 -lSDL2_ttf -lSDL2_image -no-pie

clean:
	rm -rf $(OBJ_DIR) $(BIN) $(UI_BIN)

.PHONY: all clean
