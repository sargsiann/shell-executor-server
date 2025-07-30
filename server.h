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
#include <malloc.h>
# include <pthread.h>

// Pool member for assigning to thread it will be locked that exact member of queue by the mutex
// for randomly not accessing (in the same time) from another thread
typedef struct s_queue {
	int	*connection_fd;
	struct s_queue *next;
}	t_queue;

// Functions for pool
void	add_to_pool(t_queue **pool_head,int *connection_fd);
t_queue *get_from_pool(t_queue **head) ;
void	print_pool(t_queue **head);
char	*string_reallocator(char *old_message,char *buffer) ;
char	**parser_ses(char *message) ;


#endif