# SDL Development include file and directory
SDL_DEVELOPMENT_INC := ./resource/Library/Frameworks/SDL2.framework/Headers
SDL_DEVELOPMENT_INC2 := ./resource/Library/Frameworks/SDL2_image.framework/Headers
SDL_DEVELOPMENT_DIR := ./resource/Library/Frameworks

# GNU Compiler Standard - Clang does not have standard
STD := -std=c11

# Binary File
OBJ := endgame

# Library Flags
LIBRARY := -Wl,-rpath,@executable_path/$(SDL_DEVELOPMENT_DIR) -F$(SDL_DEVELOPMENT_DIR) -framework SDL2 -framework SDL2_image

# Download Message
DOWNLOAD := `downloading SDL2_name framework library`

# Distribution
DIST_NAME := SDL2-game-osx
DIST_DEPENDENCIES := Library

# development (3), and production (0)
DEBUG := -g0

# Source Files
SRC_DIR := src/
SOURCE_FILES := src/audio.c src/display.c src/game.c src/global.c src/main.c src/object.c src/wrappers.c

SRC_DIR2 := src/
SOURCE_FILES2 := src/audio.c src/test.c

#$(wildcard $(SRC_DIR)*.c)

# Source Objects
OBJ_DIR := resource/bin/
OBJECT_FILES := $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(SOURCE_FILES))

OBJ_DIR2 := resource/bin/
OBJECT_FILES2 := $(patsubst $(SRC_DIR2)%.c,$(OBJ_DIR2)%.o,$(SOURCE_FILES2))

# Header Files
HEADER_DIR := inc/

# Depend Files
DEPEND_DIR := resource/depend/
DEPEND_FILES := $(patsubst $(SRC_DIR)%.c,$(DEPEND_DIR)%.d,$(SOURCE_FILES))

DEPEND_DIR2 := resource/depend/
DEPEND_FILES2 := $(patsubst $(SRC_DIR2)%.c,$(DEPEND_DIR2)%.d,$(SOURCE_FILES2))

# Distribution related Directories
ASSETS_DIR := resource/assets/
DIST_DIR := resource/dist/
LOGS_DIR := resource/logs/

# Compiler
CC := clang

# C Compiler Flags
CFLAGS += -Wall -Werror -Wextra -Wpedantic -pedantic -Wshadow -Wstrict-aliasing -Wstrict-overflow $(DEBUG)

# Optimizations
OPT := -O2 -flto

# Include directories
INCLUDE :=  -I$(SDL_DEVELOPMENT_INC) -I$(SDL_DEVELOPMENT_INC2) -I$(HEADER_DIR)

# Dependency Flags
DFLAGS := -MMD -MF

.PHONY: all SDL

all: SDL $(OBJ)

$(OBJ): $(OBJECT_FILES)
	@echo compiling binary \'$(OBJ)\'
	$(CC) $(DEBUG) $(STD) $(OPT) $(OBJECT_FILES) -o $(OBJ) $(LIBRARY)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) -c $< $(CFLAGS) $(STD) $(OPT) $(INCLUDE) $(DFLAGS) $(patsubst $(OBJ_DIR)%.o,$(DEPEND_DIR)%.d,$@) -o $@

box: $(OBJECT_FILES2)
	@echo compiling binary BOX
	$(CC) $(DEBUG) $(STD) $(OPT) $(OBJECT_FILES2) -o $(OBJ) $(LIBRARY)

$(OBJ_DIR2)%.o: $(SRC_DIR2)%.c
	$(CC) -c $< $(CFLAGS) $(STD) $(OPT) $(INCLUDE) $(DFLAGS) $(patsubst $(OBJ_DIR2)%.o,$(DEPEND_DIR2)%.d,$@) -o $@

# SDL Library Check
SDL:
ifeq ($(wildcard $(SDL_DEVELOPMENT_INC)),)
	$(error SDL2 development package [$(SDL_DEVELOPMENT_INC)] not found, try $(DOWNLOAD))
endif

SDL:
ifeq ($(wildcard $(SDL_DEVELOPMENT_INC2)),)
	$(error SDL2_image development package [$(SDL_DEVELOPMENT_INC2)] not found, try $(DOWNLOAD))
endif

-include $(DEPEND_FILES)

# Compile the distribution for the given system
.PHONY: dist

dist: $(DIST_DIR) $(DIST_DEPENDENCIES) $(ASSETS_DIR)
	@echo Compiling distribution build in \"$(DIST_DIR)\" as \"$(DIST_NAME).tar.gz\"
	mkdir $(DIST_DIR)$(DIST_NAME)
	mkdir $(DIST_DIR)$(DIST_NAME)/$(LOGS_DIR)
	cp $(OBJ) $(DIST_DIR)$(DIST_NAME)
	cp -r $(ASSETS_DIR) $(DIST_DIR)$(DIST_NAME)/
	cp -r $(DIST_DEPENDENCIES) $(DIST_DIR)$(DIST_NAME)/
	cp README* $(DIST_DIR)$(DIST_NAME)
	tar -zcvf $(DIST_DIR)$(DIST_NAME).tar.gz -C $(DIST_DIR) --remove-files $(DIST_NAME)

.PHONY: valgrind drmemory clean

clean:
	rm -f $(OBJ_DIR)*
	rm -f $(DEPEND_DIR)*

uninstall:
	rm -f $(OBJ_DIR)*
	rm -f $(DEPEND_DIR)*
	rm -f $(OBJ)

reinstall: uninstall all

valgrind: all
	valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes ./$(OBJ)

drmemory: all
	drmemory $(OBJ)
