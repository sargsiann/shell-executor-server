#ifndef SERVER_H
# define SERVER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <netdb.h>
# include <fcntl.h>
# include <pthread.h>

void	socket_binding(struct addrinfo *res) ;

#endif