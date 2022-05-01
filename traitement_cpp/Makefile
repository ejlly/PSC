# set the compiler
CC := clang++

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O2 -Wall -lSDL2_image -lm -std=c++20
# add header files here
HDRS := Point.h

# add source files here
SRCS := sdl_version.cpp Point.cpp

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := Sdl.out

# default recipe
all: $(EXEC)



%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

# recipe for building the final executable
$(EXEC): $(OBJS)  Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

Point.o: Point.h Point.cpp

.PHONY: all clean
