CLIENT = client

SERVER = server

CC = gcc

CL_SRC = client.c
SV_SRC = server.c

CLIENT_OBJ = $(CL_SRC:.c=.o)
SERVER_OBJ = $(SV_SRC:.c=.o)

%.o : %.c
	$(CC) -c $< -o $@

all: $(CLIENT) $(SERVER)
	@echo "Build complete: $(CLIENT) and $(SERVER) are ready."

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o client

$(SERVER): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o server

clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT) $(SERVER)

re : clean all

.PHONY: all clean re
