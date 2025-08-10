#ifndef SERVER_H
# define SERVER_H
# define BUFFER_SIZE 1024
# define THREADS_FOR_SERVE 10

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
 #include <sys/resource.h>

typedef struct sockaddr SA;

// Tasks queue for storeing clients info
typedef struct s_cinfo {
	// In each accept call getting new socket for connection
	int	c_socket_fd;
	// Struct for storeing info about client
	struct sockaddr_in6 client_info_storage;
	struct s_cinfo *next;
}	t_cinfo;

typedef	struct s_files {
	char	*name;
	struct 	s_files *next;
}	t_files;



// Thes struct for validation
typedef struct s_token {
	// The name like "ls"
	char	*token_name;

	// Type like the command,argument,operator
	short	token_type;

	struct s_token *next;
}	t_token;

t_cinfo *get_client_to_serve(t_cinfo **head) ;
void	add_client_to_serve(int client_conn_fd, t_cinfo **head) ;
void	free_queue(t_cinfo **head) ;
void	add_file(t_files **head, char *name) ;
void	free_files_list(t_files **head) ;
void	print_files_list(t_files *head);
t_token **get_token_commands(char *commands);
char	*substr(char *start, char *end) ;
void	free_tokens(t_token **head) ;
bool	validator(char *commands, char *files) ;
t_token **get_token_commands(char *commands) ;
void	print_tokens(t_token **head) ;
char	*substr(char *start, char *end);
char	*get_command(char *all_message);
char	*get_files_section(char *all_message);
char 	*str_realloc(char *old, char *new) ;
t_files	**create_files(char *files);
void	setup_signal_handler();
void	exec_command(char *command, int sock_fd) ;
void	handle_sigint(int sigint) ;
size_t	get_stack_usage(void);

#endif