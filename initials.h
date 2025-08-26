#ifndef IN_H

 #define PORT 3490
 #define BUFF_SIZE 256
 #include <string.h>
 #include <arpa/inet.h>
 #include <sys/types.h>
 #include <netdb.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <fcntl.h>
 #include <stdlib.h>
 #include <stdbool.h>

typedef struct sockaddr SA;

// For main thread server
typedef struct t_server {
	int	listen_fd;
}	t_server;

// This one for each thread
typedef	struct s_req_handler {

}	t_req_handler;

void	exit_error(const char *msg,t_server *to_free) ;
bool	is_ipv6_mapped(struct in6_addr sin6_addr) ;
void	log_client_ip_info(struct sockaddr_storage client_info) ;

#endif