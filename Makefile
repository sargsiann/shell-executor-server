CLIENT = client

SERVER = server

CC = gcc

OTHERS_SRC = thread_pool.c utils.c parser.c file_handler.c validator.c tokenizer.c
SV_SRC = server.c

SERVER_OBJ = $(SV_SRC:.c=.o)
OTHERS_OBJ = $(OTHERS_SRC:.c=.o)


%.o : %.c
	$(CC) -c $< -o $@

all: $(SERVER)

$(SERVER): $(SERVER_OBJ)  $(OTHERS_OBJ)
	$(CC) $(SERVER_OBJ) $(OTHERS_OBJ) -o server

clean:
	rm -f  $(OTHERS_OBJ) $(SERVER_OBJ) $(SERVER)

re : clean all

.PHONY: all clean re
