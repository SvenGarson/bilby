# Source files to compile
OBJS = source/*.c

# Choose compiler
CC = gcc

# Compiler flags
COMPILER_FLAGS = -Wextra -Wall

# Build name and directory
OBJ_NAME = build/bilby_driver

# Linker
LINKER_FLAGS = -lglut -lGL -lGLU

# Targets
compile : $(OBJS)
	clear
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	build/bilby_driver

compile_and_run: compile run
