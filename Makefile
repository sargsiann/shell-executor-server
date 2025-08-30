# Define the C compiler and compiler flags
CC = gcc
# CFLAGS =  -g

# Define the name of the executable
TARGET = server

# Define the source files (.c files)
SRCS = $(wildcard *.c)

# Define the object files (.o files) based on the source files
OBJS = $(SRCS:.c=.o)

# The default rule to build the server executable
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Generic rule to compile .c files into .o files
# This uses automatic variables:
# $< is the name of the prerequisite (.c file)
# $@ is the name of the target (.o file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)