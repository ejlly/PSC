# set the compiler
CC := clang++

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O2 -Wall -lSDL2_image -lm -std=c++20
# add header files here
HDRS := Point.h

# add source files here
SRCS := main.cpp Point.cpp

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := a.out

# default recipe
all: $(EXEC)
 
showfont: showfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

glfont: glfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

# recipe for building the final executable
$(EXEC): $(OBJS)  Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
