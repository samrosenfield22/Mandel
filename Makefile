#OBJS specifies your file to be compiled
OBJS = src/main.c src/mandel.c src/render.c src/keys.c

#CC specifies the compiler
CC = gcc

#COMPILER_FLAGS specifies additional compiler options
COMPILER_FLAGS = -o

#OTHER_FLAGS, if needed (ex. debug, optimization)
OTHER_FLAGS = -g -mwindows lib/glut32.lib -lopengl32 -lglu32 -O5

#EXEC specifies the executable
EXEC = bin/main.exe

all: $(OBJS)
	$(CC) $(OBJS) $(OTHER_FLAGS) $(COMPILER_FLAGS) $(EXEC)
